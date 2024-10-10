#include "commands.h"

#include <QDir>
#include <cloneparameters.h>
#include <abstractcredentialresolver.h>
#include <git2qt.h>
#include <gitexception.h>
#include <log.h>
#include <progresscallback.h>
#include <repository.h>

using namespace GIT;

QString Commands::_errorText;

Repository* Commands::clone(const QString& remoteUrl, const QString& localPath, AbstractCredentialResolver* credentialResolver, ProgressCallback* progressCallback)
{
    Git2Qt::ensureInitialized();

    CloneParameters callbacks(credentialResolver, progressCallback);

    Repository* result = nullptr;
    try
    {
        if(QDir(localPath).exists() == true) {
            throw GitException("Path exists");
        }


        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        clone_opts.bare = 0;

        clone_opts.fetch_opts.callbacks.payload = &callbacks;
        clone_opts.fetch_opts.callbacks.credentials = credentialsCallback;
        clone_opts.fetch_opts.callbacks.transfer_progress = transferProgressCallback;

        /* try to clone */
        git_repository* repo = nullptr;
        if(git_clone(&repo, remoteUrl.toUtf8().constData(), localPath.toUtf8().constData(), &clone_opts) != 0) {
            throw GitException(git_error_last()->message);
        }
        git_repository_free(repo);

        result = new Repository(localPath, false);
    }
    catch(const GitException& e)
    {
        _errorText = e.message();
        if(result != nullptr) {
            delete result;
        }
        result = nullptr;
    }
    return result;
}

Repository* Commands::cloneSubmodule(Repository* superRepo, const Submodule& submodule, AbstractCredentialResolver* credentialResolver, ProgressCallback* progressCallback)
{
    Git2Qt::ensureInitialized();

    Repository* result = nullptr;
    try
    {
        CloneParameters callbacks(credentialResolver, progressCallback);
        git_submodule_update_options clone_opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

        clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

        clone_opts.fetch_opts.callbacks.payload = &callbacks;
        clone_opts.fetch_opts.callbacks.credentials = credentialsCallback;
        clone_opts.fetch_opts.callbacks.transfer_progress = transferProgressCallback;

        SubmoduleHandle submoduleHandle = submodule.createHandle();
        throwIfTrue(superRepo, submoduleHandle.isNull());

        git_repository* repoHandle = nullptr;
        throwOnError(superRepo, git_submodule_clone(&repoHandle, submoduleHandle.value(), &clone_opts));
        result = new Repository(repoHandle);
    }
    catch(const GitException& e)
    {
        _errorText = e.message();
        superRepo->setErrorText(e.message());
        if(result != nullptr) {
            delete result;
        }
        result = nullptr;
    }
    return result;
}

bool Commands::updateSubmodule(Repository* superRepo, const Submodule& submodule, bool initialize, AbstractCredentialResolver* credentialResolver, ProgressCallback* progressCallback)
{
    bool result = false;
    try
    {
        CloneParameters callbacks(credentialResolver, progressCallback);

        git_submodule_update_options options = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

        options.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

        options.fetch_opts.callbacks.payload = &callbacks;
        options.fetch_opts.callbacks.credentials = credentialsCallback;
        options.fetch_opts.callbacks.transfer_progress = transferProgressCallback;

        SubmoduleHandle submoduleHandle = submodule.createHandle();
        throwIfTrue(superRepo, submoduleHandle.isNull());
        throwOnError(superRepo, git_submodule_update(submoduleHandle.value(), initialize, &options));
        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;

}

int Commands::credentialsCallback(git_cred** cred, const char* url, const char* username, unsigned int allowed_types, void* payload)
{
    CloneParameters* cloneParameters = static_cast<CloneParameters*>(payload);
    if(cloneParameters->credentialResolver() == nullptr) {
        return GIT_PASSTHROUGH;
    }

    Q_UNUSED(url)
    try
    {
        const char *ssh_user = username ? username : "git";
        if(allowed_types & GIT_CREDTYPE_SSH_KEY){
            if(cloneParameters->credentialResolver()->getPublicKeyFilename().isEmpty() || cloneParameters->credentialResolver()->getPrivateKeyFilename().isEmpty()) {
                throw GitException("SSH keys not set");
            }
            git_cred_ssh_key_new(cred, ssh_user,
                                 cloneParameters->credentialResolver()->getPublicKeyFilename().toUtf8().constData(),
                                 cloneParameters->credentialResolver()->getPrivateKeyFilename().toUtf8().constData(), nullptr);
        }
        else if(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
            if(cloneParameters->credentialResolver() == nullptr) {
                throw GitException("Remote is asking for username/password and no resolver is set");
            }
            AbstractCredentialResolver* resolver = cloneParameters->credentialResolver();

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

int Commands::transferProgressCallback(const git_transfer_progress* stats, void* payload)
{
    if(payload == nullptr) {
        return 0;
    }

    CloneParameters* parms = static_cast<CloneParameters*>(payload);
    if(parms != nullptr && parms->progressCallback() != nullptr) {
        parms->progressCallback()->progressCallback(stats->received_bytes, stats->received_objects, stats->total_objects);
    }

    return 0;
}
