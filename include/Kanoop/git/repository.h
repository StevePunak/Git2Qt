#ifndef REPOSITORY_H
#define REPOSITORY_H
#include <QString>
#include <git2.h>
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/branch.h>
#include <Kanoop/git/commit.h>
#include <Kanoop/git/commitoptions.h>
#include <Kanoop/git/reference.h>
#include <Kanoop/git/repositorystatus.h>
#include <Kanoop/git/checkoutoptions.h>
#include <Kanoop/git/stageoptions.h>

namespace GIT {

class TagCollection;

class SubmoduleCollection;

class ReferenceCollection;

class ObjectDatabase;
class Configuration;
class RepositoryInformation;
class StageOptions;
class Tree;
class CredentialResolver;
class AnnotatedCommitHandle;
class Diff;
class Index;
class Network;
class Remote;
class Repository : public QObject,
                   public GitEntity
{
    Q_OBJECT
public:
    explicit Repository(const QString& localPath, bool bare);
    explicit Repository(const QString& localPath);
    explicit Repository(git_repository* nativeRepo);
    virtual ~Repository();

    // Clone
    bool clone(const QString& remoteUrl, CredentialResolver* credentialResolver = nullptr);

    // Fetch
    bool fetch();

    // Push
    bool push(Branch* branch);
    bool push(const QList<Branch*> branches);
    bool push(Remote* remote, const QString& pushRefSpec);
    bool push(Remote* remote, const QStringList& pushRefSpecs);

    // Checkout
    bool checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutLocalBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree& tree, const QString& branchName, const CheckoutOptions& options = CheckoutOptions());

    // Branch creation
    Branch* createBranch(const QString& branchName, bool switchToNewBranch = false);
    Branch* createBranchFromAnnotatedCommit(const AnnotatedCommitHandle& annotatedCommit, const QString& branchName);

    // Commits
    Commit commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options = CommitOptions());
    Commit::List findCommits(const Reference& from);

    // Status
    GIT::RepositoryStatus status();

    // Stage
    void stage(const QString& path, const StageOptions& stageOptions = StageOptions());
    void stage(const QStringList& paths, const StageOptions& stageOptions = StageOptions());

    // Add
    void add(const GIT::StatusEntry::List& items);

    // Lookup
    Tree lookupTree(const ObjectId& objectId);
    Commit lookupCommit(const ObjectId& objectId);

    // Credentials Callback
    void setCredentialResolver(CredentialResolver* value) { _credentialResolver = value; }

    Branch* head() const;
    bool setHead(const QString& referenceName);

    QString localPath() const { return _localPath; }
    bool isBare() const { return _bare; }

    Branch::Map branches() const { return _branches; }
    ReferenceCollection* references() const { return _references; }
    const RepositoryHandle handle() const { return _handle; }
    Index* index() const { return _index; }
    RepositoryInformation* info() const { return _info; }
    Configuration* config() const { return _config; }
    Network* network() const { return _network; }
    Diff* diff() const { return _diff; }
    SubmoduleCollection* submodules() const { return _submodules; }
    TagCollection* tags() const { return _tags; }

    virtual bool isNull() const override { return _handle.isNull(); }

private:
    void commonInit();
    void postInitializationLookups();
    void commonDestroy();
    void emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);

    bool loadBranches();
    bool loadReferences();
    Commit::List retrieveParentsOfTheCommitBeingCreated(bool amendPreviousCommit);
    Commit::List mergeHeads();

    QString makeReferenceName(const QString& branchName);
    QString buildCommitLogMessage(const Commit& commit, bool amendPreviousCommit, bool isHeadOrphaned, bool isMergeCommit) const;
    void updateHeadAndTerminalReference(const Commit& commit, const QString& reflogMessage);

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
    CredentialResolver* _credentialResolver = nullptr;

    Branch::Map _branches;
    Commit::List _mergeHeads;

    // callbacks
public:
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);

private:
    static int progressCallback(const git_transfer_progress *stats, void *payload);
    static int mergeHeadForeachCallback(const git_oid *oid, void *payload);

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

} // namespace GIT

#endif // REPOSITORY_H
