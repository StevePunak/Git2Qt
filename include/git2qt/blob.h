/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents git blob.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef BLOB_H
#define BLOB_H

#include <git2qt/gitobject.h>

namespace GIT {
class Repository;

class GIT2QT_EXPORT Blob : public GitObject
{
public:
    Blob(Repository* repo, const ObjectId& objectId);

    QByteArray rawData();

    BlobHandle createHandle() const;

    bool isValid() const;

private:
    QByteArray _rawData;
};

} // namespace GIT

#endif // BLOB_H
