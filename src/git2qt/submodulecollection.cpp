#include "private/submodulecollection.h"

#include <gitexception.h>
#include <repository.h>
#include <submodule.h>

using namespace GIT;

SubmoduleCollection::SubmoduleCollection(Repository* repo) :
    GitEntity(SubmoduleCollectionEntity, repo)
{
    reload();
}

Submodule SubmoduleCollection::lookup(const QString& name)
{
    Submodule result;
    try
    {
        git_submodule* sub = nullptr;
        throwOnError(git_submodule_lookup(&sub, repository()->handle().value(), name.toUtf8().constData()));
        throwOnError(git_submodule_reload(sub, false));
        QString path = git_submodule_path(sub);
        QString url = git_submodule_url(sub);
        result = Submodule(repository(), name, path, url);
        _cachedSubmodules[name] = result;
    }
    catch(const GitException&)
    {
    }
    return result;
}

Submodule SubmoduleCollection::value(const QString& name)
{
    return _cachedSubmodules.value(name);
}

Submodule::Map SubmoduleCollection::values()
{
    return _cachedSubmodules;
}

void SubmoduleCollection::reload()
{
    _cachedSubmodules.clear();
    try
    {
        throwOnError(git_submodule_foreach(repository()->handle().value(), submoduleCallback, this));
    }
    catch(const GitException&)
    {
    }
}

bool SubmoduleCollection::isNull() const
{
    return repository()->isNull();
}

int SubmoduleCollection::submoduleCallback(git_submodule* sm, const char* name, void* payload)
{
    SubmoduleCollection* collection = static_cast<SubmoduleCollection*>(payload);
    QString path = git_submodule_path(sm);
    QString url = git_submodule_url(sm);
    Submodule submodule(collection->repository(), name, path, url);
    if(submodule.isNull() == false) {
        collection->_cachedSubmodules.insert(name, submodule);
        return 0;
    }
    return 1;
}

