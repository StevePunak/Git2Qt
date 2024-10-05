#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <git2.h>
#include <git2qt/declspec.h>
#include <git2qt/gitentity.h>

namespace GIT {

class Submodule;

class ProgressCallback;

class CredentialResolver;
class Repository;

class GIT2QT_EXPORT Commands : public GitEntity
{
public:
    static Repository* clone(const QString& remoteUrl, const QString& localPath, CredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);
    static Repository* cloneSubmodule(Repository* superRepo, const Submodule& submodule, CredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);
    static bool updateSubmodule(Repository* superRepo, const Submodule& submodule, bool initialize = false, CredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);

    static QString lastErrorText() { return  _errorText; }

private:
    static int credentialsCallback(git_cred **cred, const char *url, const char *username, unsigned int allowed_types, void *payload);
    static int transferProgressCallback(const git_transfer_progress *stats, void *payload);

    static QString _errorText;
};

} // namespace GIT

#endif // COMMANDS_H
