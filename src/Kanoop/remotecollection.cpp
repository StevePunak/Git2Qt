#include "remotecollection.h"

using namespace GIT;

RemoteCollection::RemoteCollection(Repository* repo) :
    GitEntity(RemoteCollectionEntity, repo)
{
}

RemoteCollection::~RemoteCollection()
{
    qDeleteAll(_remotes);
}

void RemoteCollection::append(Remote* remote)
{
    _remotes.append(remote);
}
