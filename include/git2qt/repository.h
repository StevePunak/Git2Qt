/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git repository. All interfaces to the
 * repository should got through an instance of this object.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REPOSITORY_H
#define REPOSITORY_H
#include <QString>
#include <QTimer>
#include <git2.h>
#include <git2qt/branchcollection.h>
#include <git2qt/gitentity.h>
#include <git2qt/branch.h>
#include <git2qt/diffdelta.h>
#include <git2qt/commit.h>
#include <git2qt/graphedcommit.h>
#include <git2qt/commitoptions.h>
#include <git2qt/reference.h>
#include <git2qt/remote.h>
#include <git2qt/tag.h>
#include <git2qt/repositorystatus.h>
#include <git2qt/checkoutoptions.h>
#include <git2qt/referencecollection.h>
#include <git2qt/stashcollection.h>
#include <git2qt/stageoptions.h>
#include <git2qt/submodule.h>
#include <git2qt/objectdatabase.h>
#include <git2qt/blob.h>
#include <git2qt/pulloptions.h>
#include <git2qt/statusoptions.h>
#include <git2qt/mergeresult.h>
#include <git2qt/mergeanalysisresult.h>
#include <git2qt/annotatedtag.h>
#include <git2qt/lightweighttag.h>

#include <Kanoop/timespan.h>

class QFileSystemWatcher;
namespace GIT {

class AnnotatedCommitHandle;
class AnnotatedTag;
class BranchCollection;
class CompareOptions;
class Configuration;
class AbstractCredentialResolver;
class Diff;
class Index;
class LightweightTag;
class Network;
class ObjectDatabase;
class ReferenceCollection;
class Remote;
class RepositoryInformation;
class StageOptions;
class SubmoduleCollection;
class Tag;
class TagCollection;
class Tree;

class GIT2QT_EXPORT Repository : public QObject,
                   public GitEntity
{
    Q_OBJECT
public:
    explicit Repository(const QString& localPath, bool bare);
    explicit Repository(const QString& localPath);
    explicit Repository(git_repository* nativeRepo);
    virtual ~Repository();

    static bool isRepository(const QString& path);

    // Fetch
    bool fetch(const FetchOptions& options = FetchOptions());

    // Push
    bool push(const Branch& branch);
    bool push(const Branch::List& branches);
    bool push(const Remote& remote, const QString& objectish, const QString& destinationRefSpec);
    bool push(const Remote& remote, const QString& pushRefSpec);
    bool push(const Remote& remote, const QStringList& pushRefSpecs);

    // Pull
    MergeResult pull(const Signature& merger, const PullOptions& options = PullOptions());
    bool pull2(const PullOptions& options = PullOptions());

    // Checkout
    bool checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutLocalBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree& tree, const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree& tree, const QStringList& paths, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutPaths(const QString& branchName, const QStringList& paths, const CheckoutOptions& options = CheckoutOptions());

    // Branches
    Branch createBranch(const QString& branchName, bool switchToNewBranch = false);
    Branch createBranchFromAnnotatedCommit(const AnnotatedCommitHandle& annotatedCommit, const QString& branchName);
    Branch findLocalBranch(const QString& branchName) const;
    Branch findRemoteBranch(const QString& branchName) const;
    bool deleteLocalBranch(const Reference& reference);
    Branch currentBranch();

    Branch::Map localBranches() const;
    Branch::Map remoteBranches() const;

    // Commits
    Commit commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options = CommitOptions());
    Commit findCommit(const QString& sha) { return findCommit(ObjectId(sha)); }
    Commit findCommit(const ObjectId& objectId);
    Commit findCommitFromRev(const QString& revName);
    Commit headCommit();
    Commit::List findCommits(const QString& messageRegex);
    Commit::List findCommits(const QRegularExpression& messageRegex);
    Commit::List findCommits(const Reference& from);
    Commit::List commitsFromHead();
    Commit::List allCommits(CommitSortStrategies strategy = SortStrategyTime | SortStrategyTopological);
    Commit initialCommit();
    Commit mostRecentCommit();
    int commitDistance(const Commit& a, const Commit& b);
    Commit amendCommitMessage(const Commit& commit, const QString& message);

    // Submodules
    bool addSubmodule(const QString& url, const QString& path, const CheckoutOptions& checkoutOptions = CheckoutOptions(), const FetchOptions& fetchOptions = FetchOptions());
    bool deleteSubmodule(const Submodule& submodule, bool removeFromFileSystem = true);

    // Blobs
    Blob findBlob(const ObjectId& objectId);

    // Reset
    bool resetCommit(const Commit& commit, ResetMode resetMode, const CheckoutOptions& checkoutOptions = CheckoutOptions());

    // Status
    GIT::RepositoryStatus status(const StatusOptions& options = StatusOptions());

    // Stage
    bool stage(const QString& path, const StageOptions& stageOptions = StageOptions());
    bool stage(const StatusEntry::List& entries, const StageOptions& stageOptions = StageOptions());
    bool stage(const QStringList& paths, const StageOptions& stageOptions = StageOptions());
    bool unstage(const QString& path) { return unstage(QStringList() << path); }
    bool unstage(const QStringList& paths);

    // Restore / Revert

    /**
     * @brief restore
     * Revert all the given paths to head()
     * @param paths
     * @return
     */
    bool restore(const QStringList& paths);

    // Tags
    const Tag* findTag(const QString& name) const;
    const Tag* findTag(const ObjectId& objectId) const;
    const LightweightTag* createLightweightTag(const QString& name, const GitObject& targetObject);
    const AnnotatedTag* createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject);
    bool deleteTag(const QString& tagName);

    // Lookup
    Tree lookupTree(const ObjectId& objectId);
    Tree lookupTree(const QString& sha);

    // Stash
    bool stash(const Signature& stasher, const QString& message, StashModifier options = StashModifierDefault);
    bool popStash(const Stash& stash, const StashApplyOptions& options = StashApplyOptions()) const;
    bool popStash(const ObjectId& objectId, const StashApplyOptions& options = StashApplyOptions()) const;
    bool deleteStash(const Stash& stash);
    Stash findStash(const ObjectId& objectId) const;
    Stash::List stashes() const;

    // References
    Reference::List findReferencesReachableFrom(const Commit::List& commits);
    Reference::List findReferences(const QRegularExpression& regex) const;
    bool deleteLocalReference(const Reference& reference);

    // Diffs
    DiffDelta::List diffTreeToTree_DEP(const Tree& oldTree, const Tree& newTree, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    DiffDelta::List diffCommitToCommit(const Commit& oldCommit, const Commit& newCommit, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    DiffDelta::List diffIndexToWorkDir(const QString& path, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    DiffDelta::List diffIndexToWorkDir(const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    DiffDelta::List diffTreeToWorkDir(const Tree& oldTree, const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    DiffDelta diffDelta(const StatusEntry& statusEntry) const;
    DiffDelta::List diffDeltas(const StatusEntry::List& statusEntries) const;

    // Merge
    MergeResult merge(const QList<AnnotatedCommitHandle>& handles, const Signature& merger, const MergeOptions& options);
    MergeResult mergeFetchedRefs(const Signature& merger, const MergeOptions& options);

    // Remote
    Remote::List remotes() const;
    Reference::List remoteReferences(const QString& remoteName);
    Reference::List localReferences() const;
    QString firstRemoteUrl() const;

    // Graph
    GraphedCommit::List commitGraph();

    // Tags
    AnnotatedTag::List annotatedTags() const;
    AnnotatedTag::List annotatedTags(const ObjectId& commitId) const;
    LightweightTag::List lightweightTags() const;
    LightweightTag::List lightweightTags(const ObjectId& commitId) const;

    // -- will remove these two (or make internal)
    Tag::ConstPtrList tags_DEP() const;
    QMap<ObjectId, Tag::ConstPtrList> tagsByCommitId_DEP() const;

    ObjectDatabase* objectDatabase() const { return _objectDatabase; }

    // Credentials Callback
    void setCredentialResolver(AbstractCredentialResolver* value) { _credentialResolver = value; }

    Branch head();
    bool setHead(const QString& referenceName);

    QString localPath() const { return _localPath; }
    bool isBare() const { return _bare; }

    Branch::List branches() const { return _branches->branches(); }
    Reference::List references() const;
    const RepositoryHandle handle() const { return _handle; }
    Index* index() const { return _index; }
    RepositoryInformation* info() const { return _info; }
    Configuration* config() const { return _config; }
    Network* network() const { return _network; }
    Diff* diff() const { return _diff; }
    Submodule::List submodules() const;

    void walkerTest(const ObjectId& commitId);
    void ancestorTest(const ObjectId& commitId);

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& errorText) { _errorText = errorText; }

    git_error_code errorCode() const { return (git_error_code)_errorCode; }
    void setErrorCode(int value) { _errorCode = value; }

    virtual bool isNull() const override { return _handle.isNull(); }

private:
    class BenchMark
    {
    public:
        BenchMark(int index, const TimeSpan& time) :
            _index(index), _time(time) {}

        int _index;
        TimeSpan _time;
    };
    void commonInit();
    void postInitializationLookups();
    void commonDestroy();
    void restartFileSystemWatcher();

    void emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);

    bool reloadReferences();
    Commit::List retrieveParentsOfTheCommitBeingCreated(bool amendPreviousCommit);

    Commit::List mergeHeads();
    MergeAnalysisResult mergeAnalysys(const QList<AnnotatedCommitHandle>& handles);
    MergeResult fastForwardMerge(const AnnotatedCommitHandle& annotatedCommit, const MergeOptions& options);
    MergeResult normalMerge(const QList<AnnotatedCommitHandle>& annotatedCommits, const Signature& merger, const MergeOptions& options);
    FastForwardStrategy fastForwardStrategyFromMergePreference(MergePreferences preference) const;

    QString makeReferenceName(const QString& branchName);
    QString buildCommitLogMessage(const Commit& commit, bool amendPreviousCommit, bool isHeadOrphaned, bool isMergeCommit) const;
    void updateHeadAndTerminalReference(const Commit& commit, const QString& reflogMessage);

    void startNotifyChangeTimer();

    QString _localPath;
    bool _bare = false;

    RepositoryHandle _handle;
    git_remote *_remote = nullptr;
    Index* _index = nullptr;
    Diff* _diff = nullptr;
    RepositoryInformation* _info = nullptr;
    Configuration* _config = nullptr;
    ObjectDatabase* _objectDatabase = nullptr;
    ReferenceCollection* _references = nullptr;
    Network* _network = nullptr;
    SubmoduleCollection* _submodules = nullptr;
    TagCollection* _tags = nullptr;
    AbstractCredentialResolver* _credentialResolver = nullptr;
    BranchCollection* _branches = nullptr;
    StashCollection* _stashes = nullptr;

    QFileSystemWatcher* _fileSystemWatcher = nullptr;
    QTimer _notifyChangeTimer;

    Commit::List _mergeHeads;
    QString _errorText;
    int _errorCode = 0;

    // callbacks
public:
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);

private:
    static int progressCallback(const git_transfer_progress *stats, void *payload);
    static int mergeHeadForeachCallback(const git_oid *oid, void *payload);

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
    void repositoryChanged();

private slots:
    void onFileSystemChanged(const QString&);
    void onNotifyTimerElapsed();
};

} // namespace GIT

#endif // REPOSITORY_H
