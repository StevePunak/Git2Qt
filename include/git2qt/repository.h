/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git repository. All interfacces to the
 * repository should got through an instance of this object.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REPOSITORY_H
#define REPOSITORY_H
#include <QString>
#include <git2.h>
#include <git2qt/branchcollection.h>
#include <git2qt/tagcollection.h>
#include <git2qt/gitentity.h>
#include <git2qt/branch.h>
#include <git2qt/diffdelta.h>
#include <git2qt/commit.h>
#include <git2qt/commitoptions.h>
#include <git2qt/reference.h>
#include <git2qt/repositorystatus.h>
#include <git2qt/checkoutoptions.h>
#include <git2qt/stageoptions.h>


class QFileSystemWatcher;
namespace GIT {

class AnnotatedCommitHandle;
class AnnotatedTag;
class BranchCollection;
class CompareOptions;
class Configuration;
class CredentialResolver;
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

class Repository : public QObject,
                   public GitEntity
{
    Q_OBJECT
public:
    explicit Repository(const QString& localPath, bool bare);
    explicit Repository(const QString& localPath);
    explicit Repository(git_repository* nativeRepo);
    virtual ~Repository();

    // Fetch
    bool fetch();

    // Push
    bool push(const Branch& branch);
    bool push(const Branch::List& branches);
    bool push(Remote* remote, const QString& pushRefSpec);
    bool push(Remote* remote, const QStringList& pushRefSpecs);

    // Checkout
    bool checkoutRemoteBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutLocalBranch(const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree& tree, const QString& branchName, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutTree(const Tree& tree, const QStringList& paths, const CheckoutOptions& options = CheckoutOptions());
    bool checkoutPaths(const QString& branchName, const QStringList& paths, const CheckoutOptions& options = CheckoutOptions());

    // Branch creation
    Branch createBranch(const QString& branchName, bool switchToNewBranch = false);
    Branch createBranchFromAnnotatedCommit(const AnnotatedCommitHandle& annotatedCommit, const QString& branchName);
    Branch findLocalBranch(const QString& branchName) const;

    // Commits
    Commit commit(const QString& message, const Signature& author, const Signature& committer, const CommitOptions& options = CommitOptions());
    Commit findCommit(const QString& sha) { return findCommit(ObjectId(sha)); }
    Commit findCommit(const ObjectId& objectId);
    Commit::List findCommits(const QString& messageRegex);
    Commit::List findCommits(const QRegularExpression& messageRegex);
    Commit::List findCommits(const Reference& from);
    Commit::List commitsFromHead();

    // Reset
    bool reset(const Commit& commit, ResetMode resetMode, const CheckoutOptions& checkoutOptions = CheckoutOptions());

    // Status
    GIT::RepositoryStatus status();

    // Stage
    bool stage(const QString& path, const StageOptions& stageOptions = StageOptions());
    bool stage(const StatusEntry::List& entries, const StageOptions& stageOptions = StageOptions());
    bool stage(const QStringList& paths, const StageOptions& stageOptions = StageOptions());
    bool unstage(const QString& path) { return unstage(QStringList() << path); }
    bool unstage(const QStringList& paths);

    // Add
    void add(const GIT::StatusEntry::List& items);

    // Restore
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
    Commit lookupCommit(const ObjectId& objectId);
    Commit lookupCommit(const QString& revName);

    // Diffs
    DiffDelta::List getDiffDeltas(const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone);

    // Credentials Callback
    void setCredentialResolver(CredentialResolver* value) { _credentialResolver = value; }

    Branch head() const;
    bool setHead(const QString& referenceName);

    QString localPath() const { return _localPath; }
    bool isBare() const { return _bare; }

    Branch::List branches() const { return _branches->branches(); }
    ReferenceCollection* references() const { return _references; }
    const RepositoryHandle handle() const { return _handle; }
    Index* index() const { return _index; }
    RepositoryInformation* info() const { return _info; }
    Configuration* config() const { return _config; }
    Network* network() const { return _network; }
    Diff* diff() const { return _diff; }
    SubmoduleCollection* submodules() const { return _submodules; }
    Tag::ConstPtrList tags() const { return _tags != nullptr ? _tags->tags() : Tag::ConstPtrList();  }

    virtual bool isNull() const override { return _handle.isNull(); }

private:
    void commonInit();
    void postInitializationLookups();
    void commonDestroy();
    void emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);

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
    BranchCollection* _branches = nullptr;
    QFileSystemWatcher* _fileSystemWatcher;

    Commit::List _mergeHeads;

    // callbacks
public:
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);

private:
    static int progressCallback(const git_transfer_progress *stats, void *payload);
    static int mergeHeadForeachCallback(const git_oid *oid, void *payload);

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
    void fileSystemChanged();

private slots:
    void onFileSystemChanged(const QString&);
};

} // namespace GIT

#endif // REPOSITORY_H
