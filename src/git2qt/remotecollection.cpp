#include "remotecollection.h"

using namespace GIT;

RemoteCollection::RemoteCollection(Repository* repo) :
    GitEntity(RemoteCollectionEntity, repo)
{
}

RemoteCollection::~RemoteCollection()
{
    for(Remote& remote : _remotes) {
        remote.dispose();
    }
}

void RemoteCollection::append(const Remote& remote)
{
    _remotes.append(remote);
}
