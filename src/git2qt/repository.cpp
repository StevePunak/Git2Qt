#include "repository.h"

#include "credentialresolver.cpp"

#include <QDir>
#include <branch.h>
#include <commit.h>
#include <git2qt.h>
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
#include <submodulecollection.h>
#include <QRegularExpression>
#include <gitexception.h>
#include <QFileSystemWatcher>
#include <utility.h>
#include <log.h>
#include <commitlog.h>
#include <graphedcommit.h>
#include <reflog.h>

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

Repository::Repository(git_repository* nativeRepo) :
    QObject(),
    GitEntity(GitEntity::RepositoryEntity, this)
{
    Git2Qt::ensureInitialized();
    try
    {
        _handle = RepositoryHandle(nativeRepo);
        _localPath = git_repository_path(_handle.value());
        postInitializationLookups();
    }
    catch(const GitException&)
    {
    }
}

Repository::~Repository()
{
    commonDestroy();
}

void Repository::commonInit()
{
    try
    {
        Git2Qt::ensureInitialized();
        git_repository* repo = nullptr;
        if(QDir(_localPath).exists() == false) {
            throwOnError(git_repository_init(&repo, _localPath.toUtf8().constData(), _bare));
        }
        else {
            throwOnError(git_repository_open(&repo, _localPath.toUtf8().constData()));
        }
        _handle = RepositoryHandle(repo);
        _fileSystemWatcher = new QFileSystemWatcher(QStringList() << _localPath);
        connect(_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &Repository::onFileSystemChanged);
        connect(_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &Repository::onFileSystemChanged);
        postInitializationLookups();
    }
    catch(const GitException&) {}
}

void Repository::postInitializationLookups()
{
    if(git_remote_lookup(&_remote, _handle.value(), "origin")) {
        _remote = nullptr;
    }

    _index = new Index(this);
    _diff = new Diff(this);
    _info = new RepositoryInformation(this);
    _config = new Configuration(this);
    _objectDatabase = new ObjectDatabase(this);
    _references = new ReferenceCollection(this);
    _network = new Network(this);
    _submodules = new SubmoduleCollection(this);
    _tags = new TagCollection(this);
    _branches = new BranchCollection(this);

    connect(this, &Repository::fileSystemChanged, _index, &Index::reload);
    connect(this, &Repository::fileSystemChanged, _config, &Configuration::reload);
    connect(this, &Repository::fileSystemChanged, _network, &Network::reload);
    connect(this, &Repository::fileSystemChanged, _tags, &TagCollection::reload);

    _branches->reloadBranches();

    loadReferences();
}

void Repository::commonDestroy()
{
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
    if(_submodules != nullptr) {
        delete _submodules;
        _submodules = nullptr;
    }
    if(_tags != nullptr) {
        delete _tags;
        _tags = nullptr;
    }
    if(_branches != nullptr) {
        delete _branches;
        _branches = nullptr;
    }
    delete _fileSystemWatcher;
}

bool Repository::fetch()
{
    bool result = false;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        if(_remote == nullptr) {
            throwOnError(git_remote_lookup(&_remote, _handle.value(), "origin"));
        }
        throwOnError(git_remote_fetch(_remote, nullptr, nullptr, nullptr));

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::push(const Branch& branch)
{
    Branch::List branches;
    branches.append(branch);
    return push(branches);
}

bool Repository::push(const Branch::List& branches)
{
    bool result = false;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        for(const Branch& branch : branches) {
            throwIfEmpty(branch.upstreamBranchCanonicalName());
        }

        for(const Branch& branch : branches) {
            Remote remote = _network->remoteForName(branch.remoteName());
            throwIfTrue(remote.isNull(), "remote not found for branch");
            throwIfFalse(push(remote, QString("%1:%2").arg(branch.canonicalName()).arg(branch.upstreamBranchCanonicalName())));
        }

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::push(const Remote& remote, const QString& pushRefSpec)
{
    QStringList pushRefSpecs;
    pushRefSpecs.append(pushRefSpec);
    return push(remote, pushRefSpecs);
}

bool Repository::push(const Remote& remote, const QStringList& pushRefSpecs)
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
        RemoteHandle remoteHandle = remote.createHandle();
        throwIfTrue(remoteHandle.isNull());
        throwOnError(git_remote_push(remote.createHandle().value(), strs.toNative(), &opts));

        result = true;
    }
    catch(const GitException&)
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
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        throwIfTrue(head().isNull(), "No HEAD found");
        throwIfTrue(_branches->findRemoteBranch(branchName).isNull(), "Failed to find remote branch");

        // create new branch from head
        AnnotatedCommitHandle headAnnotatedCommit = AnnotatedCommitHandle::fromRef(this, head());
        throwIfTrue(headAnnotatedCommit.isNull(), "Failed to find annotated commit for HEAD");

        if(_branches->findLocalBranch(branchName).isNull()) {
            Branch newBranch = createBranchFromAnnotatedCommit(headAnnotatedCommit, branchName);
            throwIfTrue(newBranch.isNull());
        }

        // Create and checkout tree
        Tree tree = Tree::createFromBranchName(this, branchName);
        throwIfTrue(tree.isNull(), "Failed to create tree");

        result = checkoutTree(tree, branchName, options);
    }
    catch(const GitException&)
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
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        throwIfTrue(head().isNull(), "No HEAD found");
        throwIfTrue(_branches->findLocalBranch(branchName).isNull(), "Failed to find local branch");

        // Create and checkout tree
        Tree tree = Tree::createFromBranchName(this, branchName);
        throwIfTrue(tree.isNull(), "Failed to create tree");

        result = checkoutTree(tree, branchName, options);
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutTree(const Tree& tree, const QString& branchName, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");

        ObjectHandle objectHandle = tree.createObjectHandle();
        CheckoutOptions opts = options;
        throwOnError(git_checkout_tree(_handle.value(), objectHandle.value(), opts.toNative()));
        objectHandle.dispose();

        QString referenceName = makeReferenceName(branchName);
        throwIfFalse(setHead(referenceName), "Failed to set HEAD");

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutTree(const Tree& tree, const QStringList& paths, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");

        ObjectHandle objectHandle = tree.createObjectHandle();

        CheckoutOptions opts = options;
        opts.setPaths(paths);
        throwOnError(git_checkout_tree(_handle.value(), objectHandle.value(), opts.toNative()));
        objectHandle.dispose();

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::checkoutPaths(const QString& branchName, const QStringList& paths, const CheckoutOptions& options)
{
    bool result = false;
    try
    {
        Commit commit = lookupCommit(branchName);

        checkoutTree(commit.tree(), paths, options);

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

Branch Repository::createBranch(const QString& branchName, bool switchToNewBranch)
{
    Branch result;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        throwIfTrue(head().isNull(), "No HEAD found");

        // create new branch from head
        AnnotatedCommitHandle headAnnotatedCommit = AnnotatedCommitHandle::fromRef(this, head());
        throwIfTrue(headAnnotatedCommit.isNull(), "Failed to find annotated commit for HEAD");

        Branch newBranch = createBranchFromAnnotatedCommit(headAnnotatedCommit, branchName);
        throwIfTrue(newBranch.isNull());

        if(switchToNewBranch) {
            // Create and checkout tree
            Tree tree = Tree::createFromBranchName(this, branchName);
            throwIfTrue(tree.isNull(), "Failed to create tree");
            throwIfFalse(checkoutTree(tree, branchName));
        }
        result = newBranch;
    }
    catch(const GitException&)
    {
    }
    return result;
}

Branch Repository::createBranchFromAnnotatedCommit(const AnnotatedCommitHandle& annotatedCommit, const QString& branchName)
{
    Branch result;
    git_reference* newBranch = nullptr;
    int rc = git_branch_create_from_annotated(&newBranch, _handle.value(), branchName.toUtf8().constData(), annotatedCommit.value(), false);
    if(rc == 0) {
        Reference reference = Reference::create(this, newBranch);
        result = Branch(this, reference, GIT_BRANCH_LOCAL);
        _branches->append(result);
    }
    return result;
}

Branch Repository::findLocalBranch(const QString& branchName) const
{
    return _branches->findLocalBranch(branchName);
}

Commit Repository::commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options)
{
    Commit result(this);

    try
    {
        bool orphaned = _info->isHeadUnborn();
        if(options.amendPreviousCommit() && orphaned) {
            throw GitException("Can not amend anything. The Head doesn't point at any commit.");
        }

        ObjectId treeId = _index->writeTree();
        Tree tree = lookupTree(treeId);
        if(tree.isNull()) {
            throw GitException("Failed to lookup tree");
        }

        Commit::List parents = retrieveParentsOfTheCommitBeingCreated(options.amendPreviousCommit());
        if(parents.count() == 1 && !options.allowEmptyCommit()) {
            bool treeSame = parents.at(0).treeId() == treeId;
            bool amendMergeCommit = options.amendPreviousCommit() && !orphaned && head().tip().parents().count() > 1;
            if(treeSame && !amendMergeCommit) {
                throw options.amendPreviousCommit()
                    ? GitException("Amending this commit would produce a commit that is identical to its parent")
                    : GitException("No changes; nothing to commit.");
            }
        }

        result = _objectDatabase->createCommit(author, committer, message, tree, parents, true);
        if(result.isValid() == false) {
            throw GitException("Failed to create commit");
        }

        git_repository_state_cleanup(_handle.value());

        QString logMessage = buildCommitLogMessage(result, options.amendPreviousCommit(), orphaned, parents.count() > 1);
        updateHeadAndTerminalReference(result, logMessage);

    }
    catch(const GitException&)
    {

    }
    return result;
}

Commit Repository::findCommit(const ObjectId& objectId)
{
    Commit::List allCommits = findCommits(head().reference());
    Commit result = allCommits.findCommit(objectId);
    return result;
}

Commit::List Repository::findCommits(const QString& messageRegex)
{
    return findCommits(QRegularExpression(messageRegex));
}

Commit::List Repository::findCommits(const QRegularExpression& messageRegex)
{
    Commit::List result;
    Commit::List allCommits = findCommits(head().reference());
    for(const Commit& commit : allCommits) {
        QRegularExpressionMatch match = messageRegex.match(commit.message());
        if(match.hasMatch()) {
            result.append(commit);
        }
    }
    return result;
}

Commit::List Repository::findCommits(const Reference& from)
{
    Commit::List result;

    git_oid oid;
    git_revwalk *walker = nullptr;

    try
    {
        ObjectId objid = ObjectId::createFromReference(from);
        Commit commit = Commit::lookup(this, objid);
        if(commit.isValid() == false) {
            throw GitException("Failed to find commit at starting reference");
        }

        git_revwalk_new(&walker, _handle.value());
        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
        git_revwalk_push(walker, objid.toNative());

        while(git_revwalk_next(&oid, walker) == 0) {
            commit = Commit::lookup(this, ObjectId(oid));
            if(commit.isValid() == false) {
                throw GitException("Failed to lookup the next object");
            }
            result.append(commit);
       }
    }
    catch(const GitException&)
    {

    }

    if(walker != nullptr) {
        git_revwalk_free(walker);
    }

    return result;
}

Commit::List Repository::commitsFromHead()
{
    return findCommits(head().reference());
}

Commit::List Repository::allCommits(CommitSortStrategies strategy)
{
    // set up a filter to include ALL references in time order
    CommitFilter filter;
    filter.setIncludeReachableFrom(references());
    filter.setSortBy(strategy);

    // Create a commit log with results
    CommitLog commitLog(this, filter);
    return commitLog.performLookup();
}

bool Repository::reset(const Commit& commit, ResetMode resetMode, const CheckoutOptions& checkoutOptions)
{
    bool result = false;
    ObjectHandle objectHandle = commit.createObjectHandle();
    try
    {
        CheckoutOptions opts = checkoutOptions;
        throwOnError(git_reset(_handle.value(), objectHandle.value(), (git_reset_t)resetMode, opts.toNative()));
        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    objectHandle.dispose();
    return result;
}

RepositoryStatus Repository::status()
{
    RepositoryStatus result;
    git_status_list* status_list = nullptr;

    try
    {
        throwOnError(git_index_read(_index->createHandle().value(), false));

        StatusOptions options;
        throwOnError(git_status_list_new(&status_list, _handle.value(), options.toNative()));
        int count = git_status_list_entrycount(status_list);
        for(int i = 0;i < count;i++) {
            const git_status_entry* entry = git_status_byindex(status_list, i);
            result.addStatusEntryForDelta((FileStatus)entry->status, entry->head_to_index, entry->index_to_workdir);
        }
    }
    catch(const GitException&)
    {

    }

    if(status_list != nullptr) {
        git_status_list_free(status_list);
    }
    return result;
}

bool Repository::stage(const QString& path, const StageOptions& stageOptions)
{
    QStringList paths;
    paths.append(path);
    return stage(paths, stageOptions);
}

bool Repository::stage(const StatusEntry::List& entries, const StageOptions& stageOptions)
{
    return stage(entries.paths(), stageOptions);
}

bool Repository::stage(const QStringList& paths, const StageOptions& stageOptions)
{
    bool result = false;
    try
    {
        DiffModifiers diffModifiers = DiffModIncludeUntracked;
        if(stageOptions.includeIgnored()) {
            diffModifiers |= DiffModIncludeIgnored;
        }

        CompareOptions compareOptions;
        compareOptions.setSimilarity(SimilarityOptions::none());
        TreeChanges changes = _diff->compare(diffModifiers, paths, compareOptions);

        QList<ChangeKind> validTypes = {
            ChangeKind::ChangeKindAdded,
            ChangeKind::ChangeKindModified,
            ChangeKind::ChangeKindConflicted,
            ChangeKind::ChangeKindUnmodified,
            ChangeKind::ChangeKindDeleted,
        };
        TreeChangeEntry::List unexpected = changes.getChangesNotOfKinds(validTypes);

        if(unexpected.count() > 0) {
            throw GitException(QString("Entry %1 contains an unexpected change").arg(unexpected.at(0).path()));
        }

        for(const TreeChangeEntry& change : changes) {
            switch (change.changeKind())
            {
            case ChangeKind::ChangeKindConflicted:
                if (!change.exists()) {
                    _index->remove(change.path());
                }
                break;

            case ChangeKind::ChangeKindDeleted:
                _index->remove(change.path());
                break;

            default:
                break;
            }
        }
        for(const TreeChangeEntry& change : changes) {
            switch (change.changeKind())
            {
            case ChangeKind::ChangeKindAdded:
            case ChangeKind::ChangeKindModified:
                _index->add(change.path());
                break;

            case ChangeKind::ChangeKindConflicted:
                if (change.exists()) {
                    _index->add(change.path());
                }
                break;

            default:
                break;
            }
        }

        _index->write();
        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::unstage(const QStringList& paths)
{
    bool result = false;
    try
    {
        if(_info->isHeadUnborn()) {
            // TODO
        }
        else {
            git_reference *head;
            git_object *head_commit;

            throwOnError(git_repository_head(&head, repository()->handle().value()));
            throwOnError(git_reference_peel(&head_commit, head, GIT_OBJ_COMMIT));

            StringArray str(paths);
            throwOnError(git_reset_default(repository()->handle().value(), head_commit, str.toNative()));
        }
        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

bool Repository::restore(const QStringList& paths)
{
    bool result = false;
    try
    {
        CheckoutOptions options;
        options.setModifiers(CheckoutOptions::Force);

        throwIfFalse(checkoutPaths(head().friendlyName(), paths, options));

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

const Tag* Repository::findTag(const QString& name) const
{
    return _tags->findTag(name);
}

const Tag* Repository::findTag(const ObjectId& objectId) const
{
    return _tags->findTag(objectId);
}

const LightweightTag* Repository::createLightweightTag(const QString& name, const GitObject& targetObject)
{
    return _tags->createLightweightTag(name, targetObject);
}

const AnnotatedTag* Repository::createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject)
{
    return _tags->createAnnotatedTag(name, message, signature, targetObject);
}

bool Repository::deleteTag(const QString& tagName)
{
    return _tags->deleteLocalTag(tagName);
}

Tree Repository::lookupTree(const ObjectId& objectId)
{
    Tree result;
    try
    {
        git_object* obj = nullptr;
        throwOnError(git_object_lookup(&obj, _handle.value(), objectId.toNative(), GIT_OBJECT_ANY));
        ObjectType objectType = (ObjectType)git_object_type(obj);
        switch(objectType) {
        case ObjectTypeTree:
            result = Tree(this, objectId);
            break;

        case ObjectTypeCommit:
        {
            Commit commit = findCommit(objectId);
            result = Tree(this, commit.treeId());
            break;
        }
        default:
            throw GitException(QString("Unsupported object type %1 for tree lookup").arg(getObjectTypeString(objectType)));
        }

        git_object_free(obj);
    }
    catch(const GitException&)
    {
    }
    return result;
}

Tree Repository::lookupTree(const QString& sha)
{
    return lookupTree(ObjectId(sha));
}

Commit Repository::lookupCommit(const ObjectId& objectId)
{
    Commit result = Commit::lookup(this, objectId);
    return result;
}

Commit Repository::lookupCommit(const QString& revName)
{
    Commit result;
    try
    {
        git_object* obj = nullptr;
        throwOnError(git_revparse_single(&obj, _handle.value(), revName.toUtf8().constData()));
        ObjectId objectId = git_object_id(obj);
        result = Commit::lookup(this, objectId);
    }
    catch(const GitException&)
    {
    }
    return result;
}

DiffDelta::List Repository::getDiffDeltas(const CompareOptions& compareOptions, DiffModifiers diffFlags)
{
    return _diff->listDiffs(compareOptions, diffFlags);
}

Remote::List Repository::remotes() const
{
    return _network->remotes();
}

Reference::List Repository::remoteReferences(const QString& remoteName)
{
    Reference::List references;
    Remote remote = _network->remoteForName(remoteName);
    if(remote.isNull() == false) {
        for(const Reference& reference : remote.references()) {
            if(reference.isRemote()) {
                references.append(reference);
            }
        }
    }
    return references;
}

void Repository::commitGraph()
{
    try
    {
        Branch::Map localBranches = _branches->localBranches();
        Branch::Map remoteBranches = _branches->remoteBranches();
        QMap<QString, Reflog> reflogs;
        GraphedCommit::List parsedCommits;

        // Create map of head commits
        QMap<QString, Commit> headCommits;
        for(const Branch& branch : branches()) {
            if(branch.reference().isSymbolic()) {
                continue;
            }
            Commit commit = Commit::lookup(this, branch.reference().objectId());
            throwIfFalse(commit.isValid());
            headCommits.insert(branch.friendlyName(), commit);
        }

        // Create map of branch birth commits
        QMap<QString, Commit> birthCommits;
        for(const Branch& branch : branches()) {
            if(branch.reference().isSymbolic()) {
                continue;
            }
            Reflog reflog(this, branch.canonicalName());
            if(reflog.entries().count() > 0) {
                ReflogEntry entry = reflog.entries().at(reflog.entries().count() - 1);
                Commit commit = Commit::lookup(this, entry.to());
                throwIfFalse(commit.isValid());
                birthCommits.insert(branch.friendlyName(), commit);
            }
            reflogs.insert(branch.name(), reflog);

            logText(LVL_DEBUG, QString("---------------------------------------- Reflog for %1").arg(branch.friendlyName()));
            for(const ReflogEntry& entry : reflog.entries()) {
                logText(LVL_DEBUG, QString("%1 %2").arg(entry.to().toString()).arg(entry.message()));
            }
        }

        Branch head_DEP = Repository::head();
        Commit headCommit_DEP = Commit::lookup(this, head_DEP.reference().objectId());
        throwIfFalse(headCommit_DEP.isValid());

        QString headName = head_DEP.friendlyName();
        QString currentBranchName = headName;
        QStringList branchNameStack;
        branchNameStack.append(currentBranchName);

        bool currentIsUnmerged = false;     Q_UNUSED(currentIsUnmerged)

        GraphedCommit::List originalCommits(allCommits());
        for(int i = 0; i < originalCommits.count();i++) {
            // dereference commit
            const GraphedCommit commit = originalCommits[i];
            GraphedCommit parsedCommit = commit;

            // resolve children
            GraphedCommit::List children = originalCommits.findChildren(commit);

            // Resolve branch for this commit
            QString branchTag;
            QString commitBranchName;
            Branch commitBranch;
            if((commitBranch = localBranches.findForReferencedObjectId(commit.objectId())).isValid()) {
                commitBranchName = commitBranch.friendlyName();
                parsedCommit.setHead(true);
                branchTag = QString("[%1]").arg(commitBranchName);
            }
            else if((commitBranch = remoteBranches.findForReferencedObjectId(commit.objectId())).isValid()) {
                commitBranchName = commitBranch.friendlyName(true);
                parsedCommit.setHead(true);
                branchTag = QString("[%1 (r)]").arg(commitBranch.friendlyName(true));
            }
            else {
                // we remain on the same branch
            }

            // Process branch change
            if(commitBranchName.isEmpty() == false && commitBranchName != currentBranchName) {
                // Push onto stack
                if(branchNameStack.contains(commitBranchName) == false) {
                    branchNameStack.append(commitBranchName);
                }

                if(commit.parentObjectIds().count() > 1) {
                    // This is a merge or an unmerged branch. Pop the current branch from the stack.

                    // NOT Working as intended

                    // branchNameStack.removeAll(currentBranchName);
                }
                currentBranchName = commitBranchName;
            }
            else {
                // Is the current commit reachable from the current branch head
                Commit previousCommitHead = headCommits.value(currentBranchName);
                if(previousCommitHead.isReachableFrom(commit) == false) {

                    // Find the relevant head for this commit
                    QStringList branchNames = headCommits.keys();
                    for(const QString& branchName : branchNames) {
                        Commit headCommit = headCommits.value(branchName);
                        if(commit.isReachableFrom(headCommit)) {
                            currentBranchName = branchName;
                            break;
                        }
                    }
                }
            }

            if(currentBranchName.isEmpty()) {
                currentBranchName = branchNameStack.count() > 0 ? branchNameStack.last() : "";
                parsedCommit.setHead(false);
            }

            parsedCommit.setBranchName(currentBranchName);

            // Remove any branches we have reached their birth
            QStringList branchNames = birthCommits.keys();
            for(const QString& branchName : branchNames) {
                Commit birthCommit = birthCommits.value(branchName);
                if(birthCommit.objectId() == parsedCommit.objectId()) {
                    branchNameStack.removeAll(branchName);
                }
            }

            // Detect merge
            if(parsedCommit.parents().count() > 1) {
                parsedCommit.setMerge(true);
            }

if(parsedCommit.objectId() == ObjectId("8b346920dbfd790a4c293e9993fc76bf3914a30d")) {
    int x = 1; Q_UNUSED(x)
}

            // Determine the indent level
            int level = -1;
            if((level = parsedCommits.levelForChild(parsedCommit)) > 0) {
                // do nothing... we found the level
                int x = 1; Q_UNUSED(x)
            }
            else if(currentBranchName.isEmpty() == false) {
                level = parsedCommits.levelForBranchName(currentBranchName);
            }
            if(level <= 0) {
                level = branchNameStack.count();
            }

            parsedCommit.setLevel(level);
            QString tag = "|";
            if(children.count() == 0) {
                tag = "*";
            }
            if(parsedCommit.parents().count() == 0) {
                tag = "*";
            }

            if(level > 0) {
                QString spaces = QString().leftJustified(level, ' ');
                tag = QString("%1%2%3").arg('|').arg(spaces).arg(tag);
            }

            parsedCommits.append(parsedCommit);

            logText(LVL_DEBUG, QString("%1 %2 %3 %4  %5")
                    .arg(tag)
                    .arg(parsedCommit.objectId().toString(5))
                    .arg(branchTag)
                    .arg(parsedCommit.message())
                    .arg(parsedCommit.toString()));

            // logText(LVL_DEBUG, QString("%1 has %2 parents [%3]").arg(commit.objectId().toString()).arg(commit.parents().count()).arg(commit.message()));
        }

    }
    catch(const GitException&)
    {
    }
}

GraphedCommit::List Repository::commitGraph2()
{
    GraphedCommit::List result;

    try
    {
        Branch headBranch = Repository::head();
        Commit headCommit = Commit::lookup(this, headBranch.reference().objectId());
        throwIfFalse(headCommit.isValid());

        int level = 1;

        GraphedCommit::List allCommits = Repository::allCommits(SortStrategyTime);
        for(GraphedCommit& commit : allCommits) {
            if(commit.parents().count() > 1) {
                // This is a merge
                commit.setMerge(true);

                // Find the merge-base and the merge-from
                GraphedCommit::List parents = commit.parents();
                parents.append(commit);

                Commit mergeBase = _objectDatabase->findMergeBase(parents.toCommitList());
                throwIfFalse(mergeBase.isValid());

                parents.removeAll(commit);

                commit.setMergeBase(mergeBase.objectId());
                parents.removeAll(mergeBase);

                throwIfTrue(parents.count() != 1, "Too many merge parents left (bug)");

                commit.setMergeFrom(parents.at(0).objectId());

                // Set the merge-base-of
                GraphedCommit* mergeBaseCommit = allCommits.getCommit(commit.mergeBase());
                throwIfNull(mergeBaseCommit);
                mergeBaseCommit->setMergedInto(commit.objectId());

                GraphedCommit* mergeFromCommit = allCommits.getCommit(commit.mergeFrom());
                throwIfNull(mergeFromCommit);
                mergeFromCommit->setMergedInto(commit.objectId());
            }

            if(commit.mergedInto().isValid()) {
                // Get the 'upstream' merge target. If we are not the base, increment the level
                GraphedCommit mergeTarget = allCommits.findCommit(commit.mergedInto());
                throwIfFalse(mergeTarget.isValid());

                if(mergeTarget.mergeBase() != commit.objectId()) {
                    ++level;
                    commit.setLevel(level);
                }
                else {
                    commit.setLevel(mergeTarget.level());
                }
            }
            else {
                GraphedCommit::List children = allCommits.findChildren(commit);
                if(children.count() == 0) {
                    commit.setLevel(level);
                }
                else if(children.count() == 1) {
                    level = children.at(0).level();
                    commit.setLevel(level);
                }
                else {
                    commit.setLevel(level);
                }
            }

#if 0
            // Build the output
            GraphedCommit::List children = allCommits.findChildren(commit);
            QString tag = "|";
            if(children.count() == 0) {
                tag = "*";
            }
            if(commit.parents().count() == 0) {
                tag = "*";
            }

            if(commit.level() > 0) {
                QString spaces = QString().leftJustified(commit.level(), ' ');
                tag = QString("%1%2%3").arg('|').arg(spaces).arg(tag);
            }

            logText(LVL_DEBUG, QString("%1 %2 %3 %4")
                    .arg(tag)
                    .arg(commit.objectId().toString(5))
                    .arg(commit.message())
                    .arg(commit.toString()));

#endif
        }
        result = allCommits;
    }
    catch(const GitException&)
    {
    }

    return result;
}

Branch Repository::head() const
{
    return _branches->head();
}

bool Repository::setHead(const QString& referenceName)
{
    return git_repository_set_head(_handle.value(), referenceName.toUtf8().constData()) == 0;
}

void Repository::emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    emit progress(receivedBytes, receivedObjects, totalObjects);
}

bool Repository::loadReferences()
{
    bool result = false;

    _references->clear();

    git_reference_iterator* it = nullptr;

    try
    {
        throwOnError(git_reference_iterator_new(&it, _handle.value()));
        git_reference* ref;
        while(!git_reference_next(&ref, it)) {
            Reference reference = Reference::create(this, ref);
            if(reference.isNull() == false) {
                _references->appendDirectReference(reference);
            }
        }

        _references->resolveSymbolicTargets();

        result = true;
    }
    catch(const GitException& e)
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
        result = head().tip().parents();
    }
    else {
        if(_info->isHeadUnborn() == false) {
            result.append(head().tip());
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
    git_repository_mergehead_foreach(_handle.value(), mergeHeadForeachCallback, this);
    return _mergeHeads;
}

QString Repository::makeReferenceName(const QString& branchName)
{
    return Utility::combine("refs", "heads", branchName);
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
    Reference reference = _references->head();
    if(reference.isNull()) {
        Log::logText(LVL_ERROR, "Failed to find HEAD reference!");
        return;
    }

    while(true) {
        if(reference.isDirect()) {
            _references->updateTarget(reference, commit.objectId(), reflogMessage);
            break;
        }
        else {
            Reference symRef = reference;
            if(symRef.target() == nullptr) {
                _references->appendDirectReference(symRef.targetIdentifier(), commit.objectId(), reflogMessage);
                break;
            }
            else {
                reference = *symRef.target();
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
            throw GitException("No object passed in payload (bug)");
        }

        const char *ssh_user = username ? username : "git";
        if(allowed_types & GIT_CREDTYPE_SSH_KEY){
            if(repo->_credentialResolver->getPublicKeyFilename().isEmpty() || repo->_credentialResolver->getPrivateKeyFilename().isEmpty()) {
                throw GitException("SSH keys not set");
            }
            git_cred_ssh_key_new(cred, ssh_user,
                                 repo->_credentialResolver->getPublicKeyFilename().toUtf8().constData(),
                                 repo->_credentialResolver->getPrivateKeyFilename().toUtf8().constData(), NULL);
        }
        else if(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
            if(repo->_credentialResolver == nullptr) {
                throw GitException("Remote is asking for username/password and no resolver is set");
            }
            CredentialResolver* resolver = repo->_credentialResolver;

            // Allow interactive behavior
            QString username = resolver->getUsername();
            if(username.isEmpty()) {
                throw GitException("Failed to obtain username from resolver");
            }

            QString password = resolver->getPassword();
            if(password.isEmpty()) {
                throw GitException("Failed to obtain username and password from resolver");
            }
            git_cred_userpass_plaintext_new(cred, username.toUtf8().constData(), password.toUtf8().constData());
        }
        else {
            throw GitException("Allowed type not implemented");
        }
    }
    catch(const GitException& e)
    {
        Log::logText(LVL_WARNING, e.message());
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

void Repository::onFileSystemChanged(const QString&)
{
    loadReferences();
    emit fileSystemChanged();
}
