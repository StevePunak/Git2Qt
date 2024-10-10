#include "blob.h"

#include <gitexception.h>
#include <repository.h>

using namespace GIT;

Blob::Blob(Repository* repo, const ObjectId& objectId) :
    GitObject(BlobEntity, repo, objectId)
{
}

QByteArray Blob::rawData()
{
    if(_rawData.length() > 0) {
        return _rawData;
    }

    _rawData = repository()->objectDatabase()->readBlobData(*this);
    return _rawData;
}

BlobHandle Blob::createHandle() const
{
    BlobHandle handle;
    git_blob* blob = nullptr;
    if(git_blob_lookup(&blob, repository()->handle().value(), objectId().toNative()) == 0) {
        handle = BlobHandle(blob);
    }
    return handle;
}

bool Blob::isValid() const
{
    BlobHandle handle = createHandle();
    bool result = handle.isNull() == false;
    handle.dispose();
    return result;
}
