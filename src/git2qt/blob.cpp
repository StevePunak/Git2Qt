#include "blob.h"

using namespace GIT;

Blob::Blob(Repository* repo, const ObjectId& objectId) :
    GitObject(BlobEntity, repo, objectId)
{
}
