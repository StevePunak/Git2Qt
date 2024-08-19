#include "network.h"

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

RemoteCollection* Network::remotes()
{
    return _remotes;
}

Remote* Network::remoteForName(const QString& name) const
{
    return _remotes->findByName(name);
    // git_remote_lookup()
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
            Remote* remote = new Remote(repository(), list.strings[i]);
            _remotes->append(remote);
        }
        git_strarray_free(&list);
    }
}

