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
class Repository : public GitEntity
{
    Q_OBJECT
public:
    explicit Repository(const QString& localPath, bool bare);
    explicit Repository(const QString& localPath);
    virtual ~Repository();

    // Clone
    bool clone(const QString& remoteUrl, CredentialResolver* credentialResolver = nullptr);

    // Fetch
    bool fetch();

    // Checkout
    bool checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutLocalBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree* tree, const QString& branchName, const CheckoutOptions& options = CheckoutOptions());

    // Branch creation
    Branch* createBranch(const QString& branchName, bool switchToNewBranch = false);
    Branch* createBranchFromAnnotatedCommit(AnnotatedCommitHandle* annotatedCommit, const QString& branchName);

    // Commits
    Commit commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options = CommitOptions());
    Commit::List findCommits(Reference* from);

    // Status
    GIT::RepositoryStatus status();

    // Stage
    void stage(const QString& path, const StageOptions& stageOptions = StageOptions());
    void stage(const QStringList& paths, const StageOptions& stageOptions = StageOptions());

    // Add
    void add(const GIT::StatusEntry::List& items);

    // Lookup
    Tree* lookupTree(const ObjectId& objectId);
    Commit lookupCommit(const ObjectId& objectId);

    Branch* head() const;
    bool setHead(const QString& referenceName);

    QString localPath() const { return _localPath; }
    bool isBare() const { return _bare; }

    Branch::Map branches() const { return _branches; }
    ReferenceCollection* references() const { return _references; }

    git_repository* handle() const { return _handle; }
    Index* index() const { return _index; }
    RepositoryInformation* info() const { return _info; }
    Configuration* config() const { return _config; }

    virtual bool isNull() const override { return _handle == nullptr; }

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

    git_repository* _handle = nullptr;
    git_remote *_remote = nullptr;
    Index* _index = nullptr;
    Diff* _diff = nullptr;
    RepositoryInformation* _info = nullptr;
    Configuration* _config = nullptr;
    ObjectDatabase* _objectDatabase;
    ReferenceCollection* _references;
    CredentialResolver* _credentialResolver = nullptr;

    Branch::Map _branches;
    Commit::List _mergeHeads;

    // callbacks
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);
    static int progressCallback(const git_transfer_progress *stats, void *payload);
    static int mergeHeadForeachCallback(const git_oid *oid, void *payload);

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

} // namespace GIT

#endif // REPOSITORY_H
