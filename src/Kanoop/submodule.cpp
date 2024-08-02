#include "submodule.h"

#include <repository.h>

#include <Kanoop/commonexception.h>

using namespace GIT;

Submodule::Submodule(Repository* repo, const QString& name, const QString& path, const QString& url) :
    GitEntity(SubmoduleEntity, repo),
    _name(name), _path(path), _url(url)
{
    try
    {
        SubmoudleHandle handle = createhandle();
        throwIfTrue(handle.isNull());
        _headCommitId = ObjectId(git_submodule_head_id(handle.value()));
        _indexCommitId = ObjectId(git_submodule_index_id(handle.value()));
        _workdirCommitId = ObjectId(git_submodule_wd_id(handle.value()));
        _fetchRecurseSumbmodulesRule = (SubmoduleRecurse)git_submodule_fetch_recurse_submodules(handle.value());
        _ignoreRule = (SubmoduleIgnore)git_submodule_ignore(handle.value());
        _updateRule = (SubmoduleUpdate)git_submodule_update_strategy(handle.value());
        handle.dispose();
    }
    catch(const CommonException&)
    {
    }
}

Repository* Submodule::open()
{
    Repository* repo = nullptr;
    try
    {
        SubmoudleHandle handle = createhandle();
        throwIfTrue(handle.isNull());
        git_repository* repoHandle = nullptr;
        throwOnError(git_submodule_open(&repoHandle, handle.value()));
        repo = new Repository(repoHandle);
    }
    catch(const CommonException&)
    {
    }
    return repo;
}

Submodule::SubmoduleStatuses Submodule::retrieveStatus()
{
    SubmoduleStatuses status = Unmodified;
    unsigned int stat;
    if(git_submodule_status(&stat, repository()->handle().value(), _name.toUtf8().constData(), (git_submodule_ignore_t)GitSubmoduleIgnore::Unspecified) == 0) {
        status = (SubmoduleStatuses)stat;
    }
    return status;
}

SubmoudleHandle Submodule::createhandle() const
{
    SubmoudleHandle handle;
    git_submodule* sub = nullptr;
    if(git_submodule_lookup(&sub, repository()->handle().value(), _name.toUtf8().constData()) == 0) {
        handle = SubmoudleHandle(sub);
    }
    return handle;
}
