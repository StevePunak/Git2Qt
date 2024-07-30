/**
 *  KGit
 *
 *  This class provides a wrapper of libgit2.
 *
 *  Stephen Punak, July 24 2024
 */
#ifndef KGIT_H
#define KGIT_H
#include <Kanoop/git/kanoopgit.h>
#include <QObject>
#include <git2.h>

class CredentialResolver;
class KANOOPGIT_EXPORT KGit : public QObject
{
    Q_OBJECT
public:
    KGit();

    static void ensureInitialized();

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

    void setCredentialResolver(CredentialResolver* value) { _credentialResolver = value; }

    QString errorText() const { return _errorText; }

private:
    void emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
    void throwOnError(int result) const;

    static QString makeReferenceName(const QString& branchName);

    static void initializeLibGit2();

    CredentialResolver* _credentialResolver;

    QString _publicKeyFile;
    QString _privateKeyFile;

    QString _errorText;

    static bool _git2Initialized;

#if 0
    // callbacks
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);
    static int progressCallback(const git_transfer_progress *stats, void *payload);
#endif

signals:
    void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects);
};

#endif // KGIT_H
