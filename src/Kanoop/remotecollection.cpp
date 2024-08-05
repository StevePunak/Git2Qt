#include "remotecollection.h"

using namespace GIT;

RemoteCollection::RemoteCollection(Repository* repo) :
    GitEntity(RemoteCollectionEntity, repo)
{
}

RemoteCollection::~RemoteCollection()
{
    for(Remote* remote : _remotes) {
        remote->dispose();
    }
    qDeleteAll(_remotes);
}

void RemoteCollection::append(Remote* remote)
{
    _remotes.append(remote);
}
