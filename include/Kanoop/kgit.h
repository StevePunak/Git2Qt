/**
 *  KGit
 *
 *  This class provides a wrapper of libgit2.
 *
 *  Stephen Punak, July 24 2024
 */
#ifndef KGIT_H
#define KGIT_H
#include <Kanoop/kanoopgit.h>
#include <QObject>
#include <git2.h>

class GitCredentialResolver;
class KANOOPGIT_EXPORT KGit : public QObject
{
    Q_OBJECT
public:
    KGit();

    /**
     * @brief clone
     * Clone the given repo into a local path
     * @param remoteUrl
     * @param localPath
     * @return
     */
    bool clone(const QString& remoteUrl, const QString& localPath);

    /**
     * @brief checkoutBranch
     * Checkout the named branch
     * @param localPath
     * @param branchName
     * @return
     */
    bool checkoutBranch(const QString& localPath, const QString& branchName);

    bool fetch(const QString& localPath);

    bool walkBranches(const QString& localPath);
    bool walkRefs(const QString& localPath);

    void setSshKeys(const QString& publicKeyFile, const QString& privateKeyFile) {
        _publicKeyFile = publicKeyFile;
        _privateKeyFile = privateKeyFile;
    }

    void setCredentialResolver(GitCredentialResolver* value) { _credentialResolver = value; }

    QString errorText() const { return _errorText; }

private:
    static void initializeLibGit2();
    void emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
    void throwOnError(int result) const;

    GitCredentialResolver* _credentialResolver;

    QString _publicKeyFile;
    QString _privateKeyFile;

    QString _errorText;

    static bool _git2Initialized;

    // callbacks
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);
    static int progressCallback(const git_transfer_progress *stats, void *payload);

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // KGIT_H
