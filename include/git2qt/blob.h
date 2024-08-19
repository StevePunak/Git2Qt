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

class Blob : public GitObject
{
public:
    Blob(Repository* repo, const ObjectId& objectId);
};

} // namespace GIT

#endif // BLOB_H
