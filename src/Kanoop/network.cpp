#include "network.h"

#include <remote.h>
#include <remotecollection.h>
#include <repository.h>

using namespace GIT;

Network::Network(Repository* repo) :
    GitEntity(NetworkEntity, repo)
{
    _remotes = new RemoteCollection(repo);
    git_strarray list;
    if(git_remote_list(&list, repo->handle()) == 0) {
        for(int i = 0;i < list.count;i++) {
            Remote* remote = new Remote(repo, list.strings[i]);
            _remotes->append(remote);
        }
        git_strarray_free(&list);
    }
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

