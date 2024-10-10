#include "network.h"

#include <gitexception.h>
#include <remote.h>
#include <remotecollection.h>
#include <repository.h>

using namespace GIT;

Network::Network(Repository* repo) :
    GitEntity(NetworkEntity, repo),
    _remotes(nullptr)
{
    reload();
}

Network::~Network()
{
    if(_remotes != nullptr) {
        delete _remotes;
    }
}

Remote::List Network::remotes()
{
    return _remotes->remotes();
}

Remote Network::remoteForName(const QString& name) const
{
    return _remotes->findByName(name);
    // git_remote_lookup()
}

FetchHead::List Network::fetchHeads()
{
    FetchHead::List result;
    try
    {
        throwOnError(git_repository_fetchhead_foreach(repository()->handle().value(), fetchHeadCallback, &result));
    }
    catch(const GitException&)
    {
    }

    return result;
}

void Network::reload()
{
    if(_remotes != nullptr) {
        delete _remotes;
    }

    _remotes = new RemoteCollection(repository());
    git_strarray list;
    if(git_remote_list(&list, repository()->handle().value()) == 0) {
        for(int i = 0;i < (int)list.count;i++) {
            Remote remote(repository(), list.strings[i]);
            remote.reloadReferences();
            _remotes->append(remote);
        }
        git_strarray_free(&list);
    }
}

int Network::fetchHeadCallback(const char* ref_name, const char* remote_url, const git_oid* oid, unsigned int is_merge, void* payload)
{
    FetchHead fetchHead(ref_name, remote_url, oid, is_merge);
    FetchHead::List* list = static_cast<FetchHead::List*>(payload);
    list->append(fetchHead);
    return 0;
}

