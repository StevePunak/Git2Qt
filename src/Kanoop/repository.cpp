#include "repository.h"

#include "credentialresolver.cpp"

#include <Kanoop/commonexception.h>
#include <Kanoop/datetimeutil.h>
#include <Kanoop/klog.h>
#include <Kanoop/pathutil.h>

#include <QDir>
#include <annotatedcommithandle.h>
#include <branch.h>
#include <commit.h>
#include <kgit.h>
#include <tree.h>
#include <statusentry.h>
#include <statusoptions.h>
#include <stageoptions.h>
#include <diff.h>
#include <compareoptions.h>
#include <treechanges.h>
#include <index.h>
#include <repositoryinformation.h>
#include <configuration.h>
#include <objectdatabase.h>
#include <referencecollection.h>
#include <network.h>
#include <remote.h>
#include <stringarray.h>

using namespace GIT;

Repository::Repository(const QString& localPath, bool bare) :
    QObject(),
    GitEntity(RepositoryEntity, this),
    _localPath(localPath), _bare(bare)
{
    commonInit();
}

Repository::Repository(const QString& localPath) :
    QObject(),
    GitEntity(GitEntity::RepositoryEntity, this),
    _localPath(localPath)
{
    commonInit();
}

Repository::~Repository()
{
    commonDestroy();
}

void Repository::commonInit()
{
    try
    {
        KGit::ensureInitialized();
        if(QDir(_localPath).exists() == false) {
            throwOnError(git_repository_init(&_handle, _localPath.toUtf8().constData(), _bare));
        }
        else {
            throwOnError(git_repository_open(&_handle, _localPath.toUtf8().constData()));
        }
        postInitializationLookups();
    }
    catch(const CommonException&) {}
}

void Repository::postInitializationLookups()
{
    if(git_remote_lookup(&_remote, _handle, "origin")) {
        _remote = nullptr;
    }

    _index = new Index(this);
    _diff = new Diff(this);
    _info = new RepositoryInformation(this);
    _config = new Configuration(this);
    _objectDatabase = new ObjectDatabase(this);
    _references = new ReferenceCollection(this);
    _network = new Network(this);

    loadBranches();
    loadReferences();
}

void Repository::commonDestroy()
{
    qDeleteAll(_branches);
    if(_handle != nullptr) {
        git_repository_free(_handle);
        _handle = nullptr;
    }
    if(_remote != nullptr) {
        git_remote_free(_remote);
        _remote = nullptr;
    }
    if(_index != nullptr) {
        delete _index;
        _index = nullptr;
    }
    if(_diff != nullptr) {
        delete _diff;
        _diff = nullptr;
    }
    if(_info != nullptr) {
        delete _info;
        _info = nullptr;
    }
    if(_config != nullptr) {
        delete _config;
        _config = nullptr;
    }
    if(_objectDatabase != nullptr) {
        delete _objectDatabase;
        _objectDatabase = nullptr;
    }
    if(_references != nullptr) {
        delete _references;
        _references = nullptr;
    }
    if(_network != nullptr) {
        delete _network;
        _network = nullptr;
    }
}

bool Repository::clone(const QString& remoteUrl, CredentialResolver* credentialResolver)
{
    _credentialResolver = credentialResolver;

    bool result = false;
    try
    {
        // if the directory exists, the repo must be empty and we will blow away the directory and start over
        QDir dir(_localPath);
        if(dir.exists()) {
            int res = git_repository_is_empty(_handle);
            if(res == 1) {
                if(dir.removeRecursively() == false) {
                    throw CommonException("Failed to remove directory");
                }
            }
        }

        commonDestroy();

        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        clone_opts.bare = _bare ? 1 : 0;

        clone_opts.fetch_opts.callbacks.payload = this;
        clone_opts.fetch_opts.callbacks.credentials = credentialsCallback;
        clone_opts.fetch_opts.callbacks.transfer_progress = progressCallback;

        /* try to clone */
        throwOnError(git_clone(&_handle, remoteUrl.toUtf8().constData(), _localPath.toUtf8().constData(), &clone_opts));

        // load repo objects
        postInitializationLookups();
        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::fetch()
{
    bool result = false;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");
        if(_remote == nullptr) {
            throwOnError(git_remote_lookup(&_remote, _handle, "origin"));
        }
        throwOnError(git_remote_fetch(_remote, nullptr, nullptr, nullptr));

        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::push(Branch* branch)
{
    QList<Branch*> branches;
    branches.append(branch);
    return push(branches);
}

bool Repository::push(const QList<Branch*> branches)
{
    bool result = false;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");
        for(Branch* branch : branches) {
            throwIfEmpty(branch->upstreamBranchCanonicalName());
        }

        for(Branch* branch : branches) {
            Remote* remote = _network->remoteForName(branch->remoteName());
            throwIfNull(remote, "remote not found for branch");
            throwIfFalse(push(remote, QString("%1:%2").arg(branch->canonicalName()).arg(branch->upstreamBranchCanonicalName())));
        }

        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::push(Remote* remote, const QString& pushRefSpec)
{
    QStringList pushRefSpecs;
    pushRefSpecs.append(pushRefSpec);
    return push(remote, pushRefSpecs);
}

bool Repository::push(Remote* remote, const QStringList& pushRefSpecs)
{
    bool result = false;
    try
    {
        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        callbacks.credentials = credentialsCallback;
        callbacks.payload = this;

        git_push_options opts;
        throwOnError(git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION));
        opts.callbacks = callbacks;

        StringArray strs(pushRefSpecs);
        throwOnError(git_remote_push(remote->handle(), strs.native(), &opts));

        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");
        throwIfNull(head(), "No HEAD found");
        throwIfNull(_branches.findRemoteBranch(branchName), "Failed to find remote branch");

        // create new branch from head
        AnnotatedCommitHandle* headAnnotatedCommit = AnnotatedCommitHandle::fromRef(this, head());
        throwIfNull(headAnnotatedCommit, "Failed to find annotated commit for HEAD");

        if(_branches.findLocalBranch(branchName) == nullptr) {
            Branch* newBranch = createBranchFromAnnotatedCommit(headAnnotatedCommit, branchName);
            throwIfNull(newBranch);
        }

        // Create and checkout tree
        Tree* tree = Tree::createFromBranchName(this, branchName);
        throwIfNull(tree, "Failed to create tree");

        result = checkoutTree(tree, branchName, options);
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutLocalBranch(const QString& branchName, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");
        throwIfNull(head(), "No HEAD found");
        throwIfNull(_branches.findLocalBranch(branchName), "Failed to find local branch");

        // Create and checkout tree
        Tree* tree = Tree::createFromBranchName(this, branchName);
        throwIfNull(tree, "Failed to create tree");

        result = checkoutTree(tree, branchName, options);
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutTree(const Tree* tree, const QString& branchName, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");

        throwOnError(git_checkout_tree(_handle, tree->objectHandle(), options.options()));

        QString referenceName = makeReferenceName(branchName);
        throwIfFalse(setHead(referenceName), "Failed to set HEAD");

        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }
    return result;
}

Branch* Repository::createBranch(const QString& branchName, bool switchToNewBranch)
{
    Branch* result = nullptr;
    try
    {
        throwIfNull(_handle, "Repository is not initialized");
        throwIfNull(head(), "No HEAD found");

        // create new branch from head
        AnnotatedCommitHandle* headAnnotatedCommit = AnnotatedCommitHandle::fromRef(this, head());
        throwIfNull(headAnnotatedCommit, "Failed to find annotated commit for HEAD");

        Branch* newBranch = createBranchFromAnnotatedCommit(headAnnotatedCommit, branchName);
        throwIfNull(newBranch);

        if(switchToNewBranch) {
            // Create and checkout tree
            Tree* tree = Tree::createFromBranchName(this, branchName);
            throwIfNull(tree, "Failed to create tree");

            throwIfFalse(checkoutTree(tree, branchName));

            delete tree;
        }
        result = newBranch;
    }
    catch(const CommonException&)
    {
    }
    return result;
}

Branch* Repository::createBranchFromAnnotatedCommit(AnnotatedCommitHandle* annotatedCommit, const QString& branchName)
{
    Branch* result = nullptr;
    git_reference* newBranch = nullptr;
    int rc = git_branch_create_from_annotated(&newBranch, _handle, branchName.toUtf8().constData(), annotatedCommit->handle(), false);
    if(rc == 0) {
        Reference* reference = Reference::create(this, newBranch);
        result = new Branch(this, reference, GIT_BRANCH_LOCAL);
        _branches.insert(result->name(), result);
    }
    return result;
}

Commit Repository::commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options)
{
    Commit result(this);

    try
    {
        bool orphaned = _info->isHeadUnborn();
        if(options.amendPreviousCommit() && orphaned) {
            throw CommonException("Can not amend anything. The Head doesn't point at any commit.");
        }

        ObjectId treeId = _index->writeTree();
        Tree* tree = lookupTree(treeId);
        if(tree == nullptr) {
            throw CommonException("Failed to lookup tree");
        }

        Commit::List parents = retrieveParentsOfTheCommitBeingCreated(options.amendPreviousCommit());
        if(parents.count() == 1 && !options.allowEmptyCommit()) {
            bool treeSame = parents.at(0).treeId() == treeId;
            bool amendMergeCommit = options.amendPreviousCommit() && !orphaned && head()->tip().parents().count() > 1;
            if(treeSame && !amendMergeCommit) {
                throw options.amendPreviousCommit()
                    ? CommonException("Amending this commit would produce a commit that is identical to its parent")
                    : CommonException("No changes; nothing to commit.");
            }
        }

        result = _objectDatabase->createCommit(author, committer, message, tree, parents, true);
        if(result.isValid() == false) {
            throw CommonException("Failed to create commit");
        }

        git_repository_state_cleanup(_handle);

        QString logMessage = buildCommitLogMessage(result, options.amendPreviousCommit(), orphaned, parents.count() > 1);
        updateHeadAndTerminalReference(result, logMessage);

    }
    catch(CommonException&)
    {

    }
    return result;
}

Commit::List Repository::findCommits(Reference* from)
{
    Commit::List result;

    git_oid oid;
    git_revwalk *walker = nullptr;

    try
    {
        ObjectId objid = ObjectId::createFromReference(from);
        Commit commit = Commit::lookup(this, objid);
        if(commit.isValid() == false) {
            throw CommonException("Failed to find commit at starting reference");
        }

        git_revwalk_new(&walker, _handle);
        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
        git_revwalk_push(walker, objid.toNative());

        while(git_revwalk_next(&oid, walker) == 0) {
            commit = Commit::lookup(this, ObjectId(oid));
            if(commit.isValid() == false) {
                throw CommonException("Failed to lookup the next object");
            }
            result.append(commit);
       }
    }
    catch(CommonException&)
    {

    }

    if(walker != nullptr) {
        git_revwalk_free(walker);
    }

    return result;
}

RepositoryStatus Repository::status()
{
    RepositoryStatus result;
    git_status_list* status_list = nullptr;

    try
    {
        git_index_read(_index->handle(), false);

        StatusOptions options;
        throwOnError(git_status_list_new(&status_list, _handle, options.toNative()));
        int count = git_status_list_entrycount(status_list);
        for(int i = 0;i < count;i++) {
            const git_status_entry* entry = git_status_byindex(status_list, i);
            result.addStatusEntryForDelta((FileStatus)entry->status, entry->head_to_index, entry->index_to_workdir);
        }
    }
    catch(CommonException&)
    {

    }

    if(status_list != nullptr) {
        git_status_list_free(status_list);
    }
    return result;
}

void Repository::stage(const QString& path, const StageOptions& stageOptions)
{
    QStringList paths;
    paths.append(path);
    stage(paths, stageOptions);
}

void Repository::stage(const QStringList& paths, const StageOptions& stageOptions)
{
    try
    {
        Diff::DiffModifiers diffModifiers = Diff::IncludeUntracked;
        if(stageOptions.includeIgnored()) {
            diffModifiers |= Diff::IncludeIgnored;
        }

        CompareOptions compareOptions;
        compareOptions.setSimilarity(SimilarityOptions::none());
        TreeChanges changes = _diff->compare(diffModifiers, paths, compareOptions);

        QList<ChangeKind::Kind> validTypes = {
            ChangeKind::Added,
            ChangeKind::Modified,
            ChangeKind::Conflicted,
            ChangeKind::Unmodified,
            ChangeKind::Deleted,
        };
        TreeEntryChanges::List unexpected = changes.getChangesNotOfKinds(validTypes);

        if(unexpected.count() > 0) {
            throw CommonException(QString("Entry %1 contains an unexpected change").arg(unexpected.at(0).path()));
        }

        for(const TreeEntryChanges& change : changes) {
            switch (change.status())
            {
            case ChangeKind::Conflicted:
                if (!change.exists()) {
                    _index->remove(change.path());
                }
                break;

            case ChangeKind::Deleted:
                _index->remove(change.path());
                break;

            default:
                break;
            }
        }
        for(const TreeEntryChanges& change : changes) {
            switch (change.status())
            {
            case ChangeKind::Added:
            case ChangeKind::Modified:
                _index->add(change.path());
                break;

            case ChangeKind::Conflicted:
                if (change.exists()) {
                    _index->add(change.path());
                }
                break;

            default:
                break;
            }
        }

        _index->write();
    }
    catch(CommonException&)
    {
    }
}

Tree* Repository::lookupTree(const ObjectId& objectId)
{
    Tree* result = nullptr;
    try
    {
        git_object* obj = nullptr;
        throwOnError(git_object_lookup(&obj, _handle, objectId.toNative(), GIT_OBJECT_TREE));
        result =  new Tree(this, obj);
    }
    catch(CommonException&)
    {
    }
    return result;
}

Commit Repository::lookupCommit(const ObjectId& objectId)
{
    Commit result = Commit::lookup(this, objectId);
    return result;
}

Branch* Repository::head() const
{
    for(Branch* branch : _branches) {
        if(branch->isHead()) {
            return branch;
        }
    }
    return nullptr;
}

bool Repository::setHead(const QString& referenceName)
{
    return git_repository_set_head(_handle, referenceName.toUtf8().constData()) == 0;
}

void Repository::emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    emit progress(receivedBytes, receivedObjects, totalObjects);
}

bool Repository::loadBranches()
{
    bool result = false;
    git_branch_iterator* it = nullptr;

    qDeleteAll(_branches);
    _branches.clear();

    try
    {
        throwOnError(git_branch_iterator_new(&it, _handle, GIT_BRANCH_ALL));
        git_reference* reference;
        git_branch_t type;
        while(!git_branch_next(&reference, &type, it)) {
            Branch* branch = new Branch(this, Reference::create(this, reference), type);
            _branches.insert(branch->name(), branch);
        }

        result = true;
    }
    catch(const CommonException&)
    {
        result = false;
    }

    return result;
}

bool Repository::loadReferences()
{
    bool result = false;

    _references->clear();

    git_reference_iterator* it = nullptr;

    try
    {
        throwOnError(git_reference_iterator_new(&it, _handle));
        git_reference* ref;
        while(!git_reference_next(&ref, it)) {
            Reference* reference = Reference::create(this, ref);
            if(reference != nullptr) {
                _references->append(reference);
            }
        }

        _references->resolveSymbolicTargets(this);

        result = true;
    }
    catch(const CommonException& e)
    {
        result = false;
    }

    if(it != nullptr) {
        git_reference_iterator_free(it);
    }
    return result;

}

Commit::List Repository::retrieveParentsOfTheCommitBeingCreated(bool amendPreviousCommit)
{
    Commit::List result;
    if(amendPreviousCommit) {
        result = head()->tip().parents();
    }
    else {
        if(_info->isHeadUnborn() == false) {
            result.append(head()->tip());
        }

        if(_info->currentOperation() == CurrentOperation::Merge) {
            result.append(mergeHeads());
        }
    }
    return result;
}

Commit::List Repository::mergeHeads()
{
    _mergeHeads.clear();
    git_repository_mergehead_foreach(_handle, mergeHeadForeachCallback, this);
    return _mergeHeads;
}

QString Repository::makeReferenceName(const QString& branchName)
{
    return PathUtil::combine("refs", "heads", branchName);
}

QString Repository::buildCommitLogMessage(const Commit& commit, bool amendPreviousCommit, bool isHeadOrphaned, bool isMergeCommit) const
{
    QString kind;
    if(isHeadOrphaned) {
        kind = "(initial)";
    }
    else if(amendPreviousCommit) {
        kind = "(amend)";
    }
    else if(isMergeCommit) {
        kind = "(merge)";
    }

    return QString("commit %1: %2").arg(kind).arg(commit.shortMessage());
}

void Repository::updateHeadAndTerminalReference(const Commit& commit, const QString& reflogMessage)
{
    Reference* reference = _references->head();
    if(reference == nullptr) {
        KLog::sysLogText(KLOG_ERROR, "Failed to find HEAD reference!");
        return;
    }

    while(true) {
        if(reference->isDirect()) {
            _references->updateTarget(static_cast<DirectReference*>(reference), commit.objectId(), reflogMessage);
            break;
        }
        else {
            SymbolicReference* symRef = static_cast<SymbolicReference*>(reference);
            reference = symRef->target();
            if(reference == nullptr) {
                _references->append(symRef->targetIdentifier(), commit.objectId(), reflogMessage);
                break;
            }
        }
    }
}

int Repository::credentialsCallback(git_cred** cred, const char* url, const char* username, unsigned int allowed_types, void* payload)
{
    Repository* repo = static_cast<Repository*>(payload);
    if(repo->_credentialResolver == nullptr) {
        return GIT_PASSTHROUGH;
    }

    Q_UNUSED(url)
    try
    {
        if(repo == nullptr) {
            throw CommonException("No object passed in payload (bug)");
        }

        const char *ssh_user = username ? username : "git";
        if(allowed_types & GIT_CREDTYPE_SSH_KEY){
            if(repo->_credentialResolver->getPublicKeyFilename().isEmpty() || repo->_credentialResolver->getPrivateKeyFilename().isEmpty()) {
                throw CommonException("SSH keys not set");
            }
            git_cred_ssh_key_new(cred, ssh_user,
                                 repo->_credentialResolver->getPublicKeyFilename().toUtf8().constData(),
                                 repo->_credentialResolver->getPrivateKeyFilename().toUtf8().constData(), NULL);
        }
        else if(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
            if(repo->_credentialResolver == nullptr) {
                throw CommonException("Remote is asking for username/password and no resolver is set");
            }
            CredentialResolver* resolver = repo->_credentialResolver;

            // Allow interactive behavior
            QString username = resolver->getUsername();
            if(username.isEmpty()) {
                throw CommonException("Failed to obtain username from resolver");
            }

            QString password = resolver->getPassword();
            if(password.isEmpty()) {
                throw CommonException("Failed to obtain username and password from resolver");
            }
            git_cred_userpass_plaintext_new(cred, username.toUtf8().constData(), password.toUtf8().constData());
        }
        else {
            throw CommonException("Allowed type not implemented");
        }
    }
    catch(const CommonException& e)
    {
        KLog::sysLogText(KLOG_WARNING, e.message());
        return 1;
    }
    return 0;
}

int Repository::progressCallback(const git_transfer_progress* stats, void* payload)
{
    if(payload == nullptr) {
        return 0;
    }

    Repository* repo = static_cast<Repository*>(payload);
    repo->emitProgress(stats->received_bytes, stats->received_objects, stats->total_objects);
    return 0;
}

int Repository::mergeHeadForeachCallback(const git_oid* oid, void* payload)
{
    Repository* repo = static_cast<Repository*>(payload);
    Commit commit = Commit::lookup(repo, ObjectId(oid));
    if(commit.isValid()) {
        repo->_mergeHeads.append(commit);
        return 0;
    }
    return 1;
}
