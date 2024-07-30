#include "kgit.h"

#include "credentialresolver.cpp"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>
#include <Kanoop/pathutil.h>

bool KGit::_git2Initialized = false;

KGit::KGit() :
    _credentialResolver(nullptr)
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

void KGit::ensureInitialized()
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

bool KGit::clone(const QString& remoteUrl, const QString& localPath)
{
    bool result = false;
    git_repository *repo = nullptr;

    try
    {
        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        clone_opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        clone_opts.bare = 0;

        clone_opts.fetch_opts.callbacks.payload = this;
        // clone_opts.fetch_opts.callbacks.credentials = credentialsCallback;
        // clone_opts.fetch_opts.callbacks.transfer_progress = progressCallback;

        /* try to clone */
        throwOnError(git_clone(&repo, remoteUrl.toUtf8().constData(), localPath.toUtf8().constData(), &clone_opts));
        result = true;
    }
    catch(const CommonException& e)
    {
        _errorText = e.message();
        result = false;
    }

    if(repo != nullptr) {
        git_repository_free(repo);
    }
    return result;
}

bool KGit::checkoutBranch(const QString& localPath, const QString& branchName)
{
    walkBranches(localPath);
    walkRefs(localPath);
    bool result = false;
    git_repository *repo = nullptr;
    git_reference* head = nullptr;
    git_reference* newBranch = nullptr;
    git_object *treeish = nullptr;
    git_annotated_commit* headAnnotatedCommit = nullptr;

    try
    {
        git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
        opts.checkout_strategy = GIT_CHECKOUT_SAFE;

        // load repo
        throwOnError(git_repository_init(&repo, localPath.toUtf8().constData(), false));

        // get head
        throwOnError(git_repository_head(&head, repo));

        // create new branch from head
        throwOnError(git_annotated_commit_from_ref(&headAnnotatedCommit, repo, head));
        throwOnError(git_branch_create_from_annotated(&newBranch, repo, branchName.toUtf8().constData(), headAnnotatedCommit, false));

        // check the new branch out
        throwOnError(git_revparse_single(&treeish, repo, branchName.toUtf8().constData()));
        throwOnError(git_checkout_tree(repo, treeish, &opts));

        // track remote branch
        QString refName = makeReferenceName(branchName);
        throwOnError(git_repository_set_head(repo, refName.toUtf8().constData()));

        result = true;
    }
    catch(const CommonException& e)
    {
        _errorText = e.message();
        result = false;
    }

    if(repo != nullptr)                 git_repository_free(repo);
    if(treeish != nullptr)              git_object_free(treeish);
    if(head != nullptr)                 git_reference_free(head);
    if(newBranch != nullptr)            git_reference_free(newBranch);
    if(headAnnotatedCommit != nullptr)  git_annotated_commit_free(headAnnotatedCommit);

    return result;
}

bool KGit::fetch(const QString& localPath)
{
    bool result = false;
    git_repository *repo = nullptr;
    git_remote *remote = nullptr;

    try
    {
        throwOnError(git_repository_init(&repo, localPath.toUtf8().constData(), false));
        throwOnError(git_remote_lookup(&remote, repo, "origin"));
        throwOnError(git_remote_fetch(remote, nullptr, nullptr, nullptr));

        result = true;
    }
    catch(const CommonException& e)
    {
        _errorText = e.message();
        result = false;
    }

    if(repo != nullptr) {
        git_repository_free(repo);
    }

    if(remote != nullptr) {
        git_remote_free(remote);
    }
    return result;
}

bool KGit::walkBranches(const QString& localPath)
{
    bool result = false;
    git_repository *repo = nullptr;
    git_branch_iterator* it = nullptr;

    try
    {
        throwOnError(git_repository_init(&repo, localPath.toUtf8().constData(), false));
        throwOnError(git_branch_iterator_new(&it, repo, GIT_BRANCH_ALL));
        git_reference* ref;
        git_branch_t type;
        while(!git_branch_next(&ref, &type, it)) {
            const char* name;
            git_branch_name(&name, ref);
            KLog::sysLogText(KLOG_DEBUG, QString("Branch: %1").arg(name));
            git_reference_free(ref);
        }

        result = true;
    }
    catch(const CommonException& e)
    {
        _errorText = e.message();
        result = false;
    }

    if(repo != nullptr) {
        git_repository_free(repo);
    }

    if(it != nullptr) {
        git_branch_iterator_free(it);
    }
    return result;
}

bool KGit::walkRefs(const QString& localPath)
{
    bool result = false;
    git_repository *repo = nullptr;
    git_reference_iterator* it = nullptr;

    try
    {
        throwOnError(git_repository_init(&repo, localPath.toUtf8().constData(), false));
        throwOnError(git_reference_iterator_new(&it, repo));
        git_reference* ref;
        const char* name;
        while(!git_reference_next_name(&name, it)) {
            KLog::sysLogText(KLOG_DEBUG, name);
        }

        result = true;
    }
    catch(const CommonException& e)
    {
        _errorText = e.message();
        result = false;
    }

    if(repo != nullptr) {
        git_repository_free(repo);
    }

    if(it != nullptr) {
        git_reference_iterator_free(it);
    }
    return result;
}

void KGit::initializeLibGit2()
{
    if(_git2Initialized == false) {
        git_libgit2_init();
    }
    _git2Initialized = true;
}

void KGit::emitProgress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects)
{
    emit progress(receivedBytes, receivedObjects, totalObjects);
}

void KGit::throwOnError(int result) const
{
    if(result != 0) {
        throw CommonException(git_error_last()->message);
    }
}

QString KGit::makeReferenceName(const QString& branchName)
{
    return PathUtil::combine("refs", "heads", branchName);
}

#if 0
int KGit::credentialsCallback(git_cred** cred, const char* url, const char* username, unsigned int allowed_types, void* payload)
{
    KLog::sysLogText(KLOG_DEBUG, QString("%1  url: %2  username: %3  allowed_types: %4").arg(__FUNCTION__).arg(url).arg(username).arg(allowed_types, 0, 16));
    KGit* kgit = static_cast<KGit*>(payload);
    Q_UNUSED(url)
    try
    {
        if(kgit == nullptr) {
            throw CommonException("No object passed in payload (bug)");
        }

        const char * ssh_user = username ? username : "git";
        if(allowed_types & GIT_CREDTYPE_SSH_KEY){
            if(kgit->_publicKeyFile.isEmpty() || kgit->_privateKeyFile.isEmpty()) {
                throw CommonException("SSH keys not set");
            }
            git_cred_ssh_key_new(cred, ssh_user, kgit->_publicKeyFile.toUtf8().constData(), kgit->_privateKeyFile.toUtf8().constData(), NULL);
        }
        else if(allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
            if(kgit->_credentialResolver == nullptr) {
                throw CommonException("Remote is asking for username/password and no resolver is set");
            }
            GitCredentialResolver* resolver = kgit->_credentialResolver;
            if(resolver->getUsername() == false) {
                throw CommonException("Failed to obtain username and password from resolver");
            }
            git_cred_userpass_plaintext_new(cred, resolver->getUsername().toUtf8().constData(), resolver->getPassword().toUtf8().constData());
        }
        else {
            throw CommonException("Allowed type not implemented");
        }
    }
    catch(const CommonException& e)
    {
        KLog::sysLogText(KLOG_WARNING, e.message());
        return 1;
    }
    return 0;
}

int KGit::progressCallback(const git_transfer_progress* stats, void* payload)
{
    if(payload == nullptr) {
        return 0;
    }

    KGit* git = static_cast<KGit*>(payload);
    git->emitProgress(stats->received_bytes, stats->received_objects, stats->total_objects);
    return 0;
}
#endif
