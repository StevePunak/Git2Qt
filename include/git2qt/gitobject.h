/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Base class for blob, commit, tag, annotation and tree objects.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GITOBJECT_H
#define GITOBJECT_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>
#include <git2qt/handle.h>

namespace GIT {

class Repository;
class GIT2QT_EXPORT GitObject : public GitEntity
{
protected:
    GitObject(GitEntityType type, Repository* repo, const ObjectId& objectId);
    GitObject(GitEntityType type, Repository* repo, const git_object* nativeObject);

public:
    GitObject(const GitObject& other);
    GitObject& operator=(const GitObject& other);

    ObjectId objectId() const { return _objectId; }

    ObjectHandle createObjectHandle() const;

    virtual bool isNull() const override { return _objectId.isNull(); }

    static ObjectType getObjectType(Repository* repo, const ObjectId& objectId);

protected:
    void setObjectId(const ObjectId& value) { _objectId = value; }

private:
    ObjectId _objectId;
};

} // namespace GIT

#endif // GITOBJECT_H
