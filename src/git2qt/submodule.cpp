#include "submodule.h"

#include <commands.h>
#include <gitexception.h>
#include <repository.h>

using namespace GIT;

const Submodule::SubmoduleStatusToStringMap Submodule::_SubmoduleStatusToStringMap;

Submodule::Submodule(Repository* repo, const QString& name, const QString& path, const QString& url) :
    GitEntity(SubmoduleEntity, repo),
    _name(name), _path(path), _url(url)
{
    try
    {
        SubmoduleHandle handle = createHandle();
        throwIfTrue(handle.isNull());
        _headCommitId = ObjectId(git_submodule_head_id(handle.value()));
        _indexCommitId = ObjectId(git_submodule_index_id(handle.value()));
        _workdirCommitId = ObjectId(git_submodule_wd_id(handle.value()));
        _fetchRecurseSumbmodulesRule = (SubmoduleRecurse)git_submodule_fetch_recurse_submodules(handle.value());
        _ignoreRule = (SubmoduleIgnore)git_submodule_ignore(handle.value());
        _updateRule = (SubmoduleUpdate)git_submodule_update_strategy(handle.value());
        handle.dispose();
    }
    catch(const GitException&)
    {
    }
}

bool Submodule::isWorkdirInitialized() const
{
    return (status() & (WorkDirUninitialized | WorkDirDeleted)) == 0;
}

bool Submodule::initialize(bool overwrite)
{
    bool result = false;
    try
    {
        SubmoduleHandle handle = createHandle();
        throwIfTrue(handle.isNull());
        throwOnError(git_submodule_init(handle.value(), overwrite));
        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

Repository* Submodule::open()
{
    Repository* repo = nullptr;
    try
    {
        SubmoduleHandle handle = createHandle();
        throwIfTrue(handle.isNull());
        git_repository* repoHandle = nullptr;
        throwOnError(git_submodule_open(&repoHandle, handle.value()));
        repo = new Repository(repoHandle);
    }
    catch(const GitException&)
    {
    }
    return repo;
}

Repository* Submodule::clone(CredentialResolver* credentialResolver, ProgressCallback* progressCallback)
{
    Repository* repo = Commands::cloneSubmodule(repository(), *this, credentialResolver, progressCallback);
    return repo;
}

bool Submodule::update(bool initialize, CredentialResolver* credentialResolver, ProgressCallback* progressCallback)
{
    bool result = Commands::updateSubmodule(repository(), *this, initialize, credentialResolver, progressCallback);
    return result;
}

Submodule::SubmoduleStatuses Submodule::status() const
{
    SubmoduleStatuses status = Unmodified;
    unsigned int stat;
    if(git_submodule_status(&stat, repository()->handle().value(), _name.toUtf8().constData(), (git_submodule_ignore_t)GitSubmoduleIgnore::Unspecified) == 0) {
        status = (SubmoduleStatuses)stat;
    }
    return status;
}

SubmoduleHandle Submodule::createHandle() const
{
    SubmoduleHandle handle;
    git_submodule* sub = nullptr;
    if(git_submodule_lookup(&sub, repository()->handle().value(), _name.toUtf8().constData()) == 0) {
        handle = SubmoduleHandle(sub);
    }
    return handle;
}

QString GIT::getSubmoduleStatusString(Submodule::SubmoduleStatus value)
{
    return Submodule::getSubmoduleStatusString(value);
}

Submodule::SubmoduleStatus GIT::getSubmoduleStatus(const QString& value)
{
    return Submodule::getSubmoduleStatus(value);
}

QList<Submodule::SubmoduleStatus> GIT::getSubmoduleStatusValues()
{
    return Submodule::getSubmoduleStatusValues();
}
