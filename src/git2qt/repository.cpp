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
#include <git2qt/private/submodulecollection.h>
#include <QRegularExpression>
#include <gitexception.h>
#include <QFileSystemWatcher>
#include <utility.h>
#include <log.h>
#include <commitlog.h>
#include <graphedcommit.h>
#include <reflog.h>
#include <QDirIterator>
#include <QElapsedTimer>

#include <git2qt/private/graphbuilder.h>

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
    GitEntity(RepositoryEntity, this),
    _localPath(localPath)
{
    commonInit();
}

Repository::Repository(git_repository* nativeRepo) :
    QObject(),
    GitEntity(RepositoryEntity, this)
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

bool Repository::isRepository(const QString& path)
{
    Git2Qt::ensureInitialized();
    int rc = git_repository_open_ext(nullptr, path.toUtf8().constData(), GIT_REPOSITORY_OPEN_NO_SEARCH, nullptr);
    bool result = rc == 0;
    return result;
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

        // initialize all objects
        postInitializationLookups();

        // connect notify timer
        connect(&_notifyChangeTimer, &QTimer::timeout, this, &Repository::onNotifyTimerElapsed);
        _notifyChangeTimer.setSingleShot(true);

        // inotify etc
        restartFileSystemWatcher();

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
    _stashes = new StashCollection(this);

    connect(this, &Repository::repositoryChanged, _index, &Index::reload);
    connect(this, &Repository::repositoryChanged, _config, &Configuration::reload);
    connect(this, &Repository::repositoryChanged, _network, &Network::reload);
    connect(this, &Repository::repositoryChanged, _tags, &TagCollection::reload);

    _branches->reloadBranches();

    reloadReferences();
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
    if(_stashes != nullptr) {
        delete _stashes;
        _stashes = nullptr;
    }
    delete _fileSystemWatcher;
}

void Repository::restartFileSystemWatcher()
{
    QStringList dirsToWatch;
    QStringList filesToWatch;
    IndexEntry::List entries = _index->entries();
    for(const IndexEntry& entry : entries) {
        QString fullPath = Utility::combine(_localPath, entry.path());
        QFileInfo fileInfo(fullPath);
        if(dirsToWatch.contains(fileInfo.absolutePath()) == false) {
            dirsToWatch.append(fileInfo.absolutePath());
            // logText(LVL_DEBUG, QString("Watch dir %1").arg(fileInfo.absolutePath()));
        }
        filesToWatch.append(fullPath);
        // logText(LVL_DEBUG, QString("Watch file %1").arg(fullPath));
    }

    if(_fileSystemWatcher != nullptr) {
        delete _fileSystemWatcher;
    }
    filesToWatch.append(dirsToWatch);
    _fileSystemWatcher = new QFileSystemWatcher(filesToWatch);
    connect(_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &Repository::onFileSystemChanged);
    connect(_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &Repository::onFileSystemChanged);
}

bool Repository::fetch(const FetchOptions& options)
{
    bool result = false;
    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        if(_remote == nullptr) {
            throwOnError(git_remote_lookup(&_remote, _handle.value(), "origin"));
        }
        FetchOptions opts = options;
        throwOnError(git_remote_fetch(_remote, nullptr, opts.toNative(), nullptr));

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

bool Repository::push(const Remote& remote, const QString& objectish, const QString& destinationRefSpec)
{
    // This signature pushes the remote for a local branch which does
    // not yet have a branch on the remote/
    // Taken from libgit2sharp
    QString refspecs = QString("%1:%2").arg(objectish).arg(destinationRefSpec);
    return push(remote, refspecs);
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
    RemoteHandle remoteHandle = remote.createHandle();
    try
    {
        throwIfTrue(remoteHandle.isNull());
        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        callbacks.credentials = credentialsCallback;
        callbacks.payload = this;

        git_push_options opts;
        throwOnError(git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION));
        opts.callbacks = callbacks;

        StringArray strs(pushRefSpecs);
        throwOnError(git_remote_push(remoteHandle.value(), strs.toNative(), &opts));

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    remoteHandle.dispose();
    reloadReferences();
    return result;
}

bool Repository::pull(const PullOptions& options)
{
    // NOT SURE THIS IS RIGHT - Git2Sharp is quite different

    bool result = false;
    git_annotated_commit* theirHead = nullptr;
    try
    {
        // Fetch
        throwIfFalse(fetch(options.fetchOptions()));

        // Get the remotes HEAD
        throwOnError(git_annotated_commit_from_revspec(&theirHead, _handle.value(), "FETCH_HEAD"));

        git_merge_analysis_t analysis;
        git_merge_preference_t preference;
        throwOnError(git_merge_analysis(&analysis, &preference, _handle.value(), (const git_annotated_commit**)&theirHead, 1));

        // Merge the remote's HEAD into the current branch
        MergeOptions mergeOpts = options.mergeOptions();
        CheckoutOptions checkoutOpts;
        throwOnError(git_merge(_handle.value(), (const git_annotated_commit**)&theirHead, 1, mergeOpts.toNative(), checkoutOpts.toNative()));

        // Checkout the changes
        throwOnError(git_checkout_head(_handle.value(), checkoutOpts.toNative()));

        reloadReferences();

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }

    if(theirHead != nullptr) {
        git_annotated_commit_free(theirHead);
    }
    return result;
}

bool Repository::checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options)
{
    bool result = false;
    QString name = Branch::removeOrigin(branchName);

    try
    {
        throwIfTrue(_handle.isNull(), "Repository is not initialized");
        throwIfTrue(head().isNull(), "No HEAD found");
        throwIfTrue(_branches->findRemoteBranch(name).isNull(), "Failed to find remote branch");

        // create new branch from head
        AnnotatedCommitHandle headAnnotatedCommit = AnnotatedCommitHandle::fromRef(this, head());
        throwIfTrue(headAnnotatedCommit.isNull(), "Failed to find annotated commit for HEAD");

        if(_branches->findLocalBranch(name).isNull()) {
            Branch newBranch = createBranchFromAnnotatedCommit(headAnnotatedCommit, name);
            throwIfTrue(newBranch.isNull());
        }

        // Create and checkout tree
        Tree tree = Tree::createFromBranchName(this, name);
        throwIfTrue(tree.isNull(), "Failed to create tree");

        result = checkoutTree(tree, name, options);
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
        Branch branch = _branches->findLocalBranch(branchName);
        throwIfTrue(branch.isNull(), "Failed to find local branch");
        throwIfTrue(branch.tip().isNull(), "The tip is null. There is nothing to check out.");

        // TODO: Need to handle detached head

#if 0
        // Create and checkout tree
        Tree tree = Tree::createFromBranchName(this, branchName);
        throwIfTrue(tree.isNull(), "Failed to create tree");
#endif

        result = checkoutTree(branch.tip().tree(), branchName, options);
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
        Commit commit = findCommit(branchName);

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
        reloadReferences();
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
        result = Branch(this, reference);
        _branches->append(result);
        reloadReferences();
    }
    return result;
}

Branch Repository::findLocalBranch(const QString& branchName) const
{
    return _branches->findLocalBranch(branchName);
}

bool Repository::deleteLocalBranch(const Reference &reference)
{
    bool result = false;
    ReferenceHandle refHandle = reference.createHandle();
    try
    {
        throwOnError(refHandle.isNull(), "Invalid reference");
        throwIfTrue(git_branch_is_checked_out(refHandle.value()) == 1, "Can't delete the checked out branch");
        throwOnError(git_branch_delete(refHandle.value()));
        reloadReferences();
        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

Branch Repository::currentBranch()
{
    Branch result;
    try
    {
        Reference head = _references->head();
        throwIfTrue(head.isNull());
        if(head.isSymbolic()) {
            if(head.target() == nullptr) {
                logText(LVL_DEBUG, "Periodic crash.... Figure this out when it becomes reproducible (10/4/24)");
            }
            Reference resolved = *head.target();
            result = Branch(this, resolved);
        }
        else {
            result = Branch(this, head);
            result.setDetachedHead(true);
        }
    }
    catch(const GitException&)
    {
    }
    return result;
}

Branch::Map Repository::localBranches() const
{
    return _branches->localBranches();
}

Branch::Map Repository::remoteBranches() const
{
    return _branches->remoteBranches();
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
    // Commit result = allCommits().findCommit(objectId);
    Commit result = Commit::lookup(this, objectId);
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

Commit Repository::findCommitFromRev(const QString& revName)
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

Commit Repository::headCommit()
{
    Commit result;
    try
    {
        git_oid headOid;
        throwOnError(git_reference_name_to_id(&headOid, _handle.value(), "HEAD"));
        result = Commit(this, ObjectId(headOid));
    }
    catch(const GitException&)
    {
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
    ObjectId::List oids;
    oids.append(headCommit().objectId());
    oids.append(mostRecentCommit().objectId());
    oids.append(references().objectIds());
    filter.setIncludeReachableFrom(oids);
    filter.setSortBy(strategy);

    // Create a commit log with results
    CommitLog commitLog(this, filter);
    return commitLog.performLookup();
}

Commit Repository::initialCommit()
{
    // set up a filter to include ALL references in time order
    CommitFilter filter;
    filter.setSortBy(SortStrategyTime | SortStrategyReverse);
    filter.setIncludeReachableFrom(headCommit().objectId());
    filter.setMaxResults(1);

    // Create a commit log with results
    CommitLog commitLog(this, filter);
    Commit::List results = commitLog.performLookup();
    Commit result;
    if(results.count() > 0) {
        result = results.at(0);
    }
    return result;
}

Commit Repository::mostRecentCommit()
{
    // set up a filter to include ALL references in time order
    CommitFilter filter;
    filter.setIncludeReachableFrom(references());
    filter.setSortBy(SortStrategyTime);
    filter.setMaxResults(1);

    // Create a commit log with results
    CommitLog commitLog(this, filter);
    Commit::List results = commitLog.performLookup();
    Commit result;
    if(results.count() > 0) {
        result = results.at(0);
    }
    return result;
}

int Repository::commitDistance(const Commit& a, const Commit& b)
{
    CommitFilter filter;
    filter.setIncludeReachableFrom(a.objectId());
    filter.setIncludeReachableFrom(b.objectId());
    filter.setSortBy(SortStrategyTime);

    // Create a commit log with results
    CommitLog commitLog(this, filter);
    Commit::List results = commitLog.performLookup();
    int idx1 = results.indexOfObjectId(a.objectId());
    int idx2 = results.indexOfObjectId(b.objectId());
    int result = -1;
    if(idx1 >= 0 && idx2 >= 0) {
        result = std::abs(idx1 - idx2);
    }
    return result;
}

Blob Repository::findBlob(const ObjectId& objectId)
{
    Blob result(this, objectId);
    return result;
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
            StatusEntry statusEntry = result.addStatusEntryForDelta((FileStatus)entry->status, entry->head_to_index, entry->index_to_workdir);
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
        startNotifyChangeTimer();
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
        startNotifyChangeTimer();
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

bool Repository::stash(const Signature& stasher, const QString& message, StashModifier options)
{
    return _stashes->addStash(stasher, message, options);
}

bool Repository::popStash(const Stash& stash, const StashApplyOptions& options) const
{
    return popStash(stash.workTree().objectId(), options);
}

bool Repository::popStash(const ObjectId& objectId, const StashApplyOptions& options) const
{
    return _stashes->popStash(objectId, options);
}

bool Repository::deleteStash(const Stash& stash)
{
    bool result = _stashes->deleteStash(stash.workTree().objectId());
    emit repositoryChanged();
    return result;
}

Stash Repository::findStash(const ObjectId& objectId) const
{
    return _stashes->findStash(objectId);
}

Stash::List Repository::stashes() const
{
    return _stashes->stashes();
}

DiffDelta::List Repository::diffTreeToTree(const Tree& fromTree, const Tree& newTree, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    return _diff->diffTreeToTree(fromTree, newTree, compareOptions, diffFlags);
}

DiffDelta::List Repository::diffIndexToWorkDir(const QString& path, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    return _diff->diffIndexToWorkDir(path, includeUntracked, compareOptions, diffFlags);
}

DiffDelta::List Repository::diffIndexToWorkDir(const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    return _diff->diffIndexToWorkDir(paths, includeUntracked, compareOptions, diffFlags);
}

DiffDelta::List Repository::diffTreeToWorkDir(const Tree& oldTree, const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    return _diff->diffTreeToWorkDir(oldTree, paths, includeUntracked, compareOptions, diffFlags);
}

DiffDelta Repository::diffDelta(const StatusEntry& statusEntry) const
{
    StatusEntry::List statusEntries;
    statusEntries.append(statusEntry);

    DiffDelta::List deltas = diffDeltas(statusEntries);
    DiffDelta result;
    if(deltas.count() == 1) {
        result = deltas.at(0);
    }
    else if(deltas.count() > 0) {
        logText(LVL_WARNING, "Too many deltas returned. Using first");
        result = deltas.at(0);
    }
    return result;
}

DiffDelta::List Repository::diffDeltas(const StatusEntry::List& statusEntries) const
{
    DiffDelta::List deltas;
    CompareOptions compareOptions;
    compareOptions.setSimilarity(SimilarityOptions::defaultOptions());
    compareOptions.setContextLines(0);
    DiffDelta::List allDeltas = diffIndexToWorkDir("*", true, compareOptions);

    for(const StatusEntry& statusEntry : statusEntries) {
        DiffDelta delta = allDeltas.findFirstByPath(statusEntry.path());
        if(delta.isValid() == false) {
            logText(LVL_WARNING, "Failed to find delta");
            continue;
        }

        // Handle rename
        if(statusEntry.status() == RenamedInWorkdir) {
            DiffDelta oldFileDelta = allDeltas.findFirstByOldFileId(delta.newFile().objectId());
            if(oldFileDelta.isValid() == false) {
                logText(LVL_WARNING, "Failed to find old file delta");
            }
            delta.setOldFile(oldFileDelta.oldFile());
        }

        deltas.append(delta);
    }

    return deltas;
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

QString Repository::firstRemoteUrl() const
{
    QString result;
    if(remotes().count() > 0) {
        result = remotes().at(0).url();
    }
    return result;
}

GraphedCommit::List Repository::commitGraph()
{
    GraphedCommit::List result;

    try
    {
        GraphBuilder graphBuilder(this);
        graphBuilder.calculateGraph();
        result = graphBuilder.graphedCommits();
    }
    catch(const GitException&)
    {
    }

    return result;
}

Branch Repository::head()
{
    Branch branch;
    Reference ref = _references->head();
    if(ref.isNull()) {
        logText(LVL_ERROR, "Corrupt repository. The 'HEAD' reference is missing.");
    }
    else if(ref.isSymbolic()) {
        branch = Branch(this, ref);
    }
    else {
        branch = Branch(this, ref);
        branch.setDetachedHead(true);
    }
    return branch;
}

bool Repository::setHead(const QString& referenceName)
{
    return git_repository_set_head(_handle.value(), referenceName.toUtf8().constData()) == 0;
}

Reference::List Repository::references() const
{
    Reference::List references = _references->references();
    Reference head = _references->head();
    if(head.isNull() == false && references.contains(head) == false) {
        references.prepend(head);
    }
    return references;
}

Submodule::List Repository::submodules() const
{
    return _submodules->values().values();
}

void Repository::walkerTest(const ObjectId &commitId)
{
    Q_UNUSED(commitId)
    QTextStream out(stdout);

    try
    {
        Commit first = mostRecentCommit();
        git_revwalk* walker = nullptr;
        throwOnError(git_revwalk_new(&walker, _handle.value()));
        throwOnError(git_revwalk_sorting(walker, GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL));

        throwOnError(git_revwalk_push(walker, first.objectId().toNative()));
        throwOnError(git_revwalk_push_head(walker));

        git_oid oid;
        while(git_revwalk_next(&oid, walker) == 0) {
            ObjectId commitId(oid);
            Commit commit(this, commitId);
            out << QString("%1 %2").arg(commit.objectId().toString()).arg(commit.shortMessage().trimmed()) << Qt::endl;
        }
    }
    catch(const GitException&)
    {
    }

}

void Repository::ancestorTest(const ObjectId &commitId)
{
    GraphBuilder builder(this);
    builder.ancestorTest(commitId);
}

void Repository::emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    emit progress(receivedBytes, receivedObjects, totalObjects);
}

bool Repository::reloadReferences()
{
    bool result = false;

    _references->clear();

    git_reference_iterator* it = nullptr;

    try
    {
        // iterate through local references
        throwOnError(git_reference_iterator_new(&it, _handle.value()));
        git_reference* ref;
        while(!git_reference_next(&ref, it)) {
            Reference reference = Reference::create(this, ref);
            if(reference.isNull() == false) {
                _references->appendDirectReference(reference);
            }
        }

        _references->resolveSymbolicTargets();

        // reload remotes
        _network->reload();

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

void Repository::startNotifyChangeTimer()
{
    _notifyChangeTimer.start(100);      // debounce
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
    startNotifyChangeTimer();
}

void Repository::onNotifyTimerElapsed()
{
    logText(LVL_DEBUG, __FUNCTION__);
    reloadReferences();
    emit repositoryChanged();
}
