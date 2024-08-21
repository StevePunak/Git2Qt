#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <git2.h>

namespace GIT {

class ProgressCallback;

class CredentialResolver;
class Repository;

class Commands
{
public:
    static Repository* clone(const QString& remoteUrl, const QString& localPath, CredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);

    static QString lastErrorText() { return  _errorText; }

private:
    class CloneParameters
    {
    public:
        CloneParameters(CredentialResolver* credentialResolver, ProgressCallback* progressCallback) :
            _credentialResolver(credentialResolver), _progressCallback(progressCallback) {}

        CredentialResolver* credentialResolver() const { return _credentialResolver; }
        ProgressCallback* progressCallback() const { return _progressCallback; }

        QString errorText() const { return _errorText; }
        void setErrorText(const QString& value) { _errorText = value; }

    private:
        CredentialResolver* _credentialResolver;
        ProgressCallback* _progressCallback;
        QString _errorText;
    };

    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);
    static int transferProgressCallback(const git_transfer_progress *stats, void *payload);

    static QString _errorText;
};

} // namespace GIT

#endif // COMMANDS_H
