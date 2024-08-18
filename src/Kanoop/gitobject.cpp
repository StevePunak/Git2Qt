#include "gitobject.h"
#include "repository.h"

using namespace GIT;

GitObject::GitObject(GitEntityType type, Repository* repo, const ObjectId& objectId) :
    GitEntity(type, repo),
    _objectId(objectId)
{
}

GitObject::GitObject(GitEntityType type, Repository* repo, const git_object* nativeObject) :
    GitEntity(type, repo),
    _objectId(nativeObject)
{
}

GitObject::GitObject(const GitObject& other) :
    GitEntity(other.entityType(), other.repository()),
    _objectId(other._objectId)
{
    *this = other;
}

GitObject& GitObject::operator=(const GitObject& other)
{
    GitEntity::operator =(other);
    _objectId = other._objectId;
    return *this;
}

ObjectHandle GitObject::createObjectHandle() const
{
    ObjectHandle handle;
    git_object* obj = nullptr;
    if(git_object_lookup(&obj, repository()->handle().value(), objectId().toNative(), GIT_OBJECT_ANY) == 0) {
        handle = ObjectHandle(obj);
    }
    return handle;
}

ObjectType GitObject::getObjectType(Repository* repo, const ObjectId& objectId)
{
    ObjectType result = ObjectTypeInvalid;
    git_object* obj = nullptr;
    if(git_object_lookup(&obj, repo->handle().value(), objectId.toNative(), GIT_OBJECT_ANY) == 0) {
        result = (ObjectType)git_object_type(obj);
        git_object_free(obj);
    }
    return result;
}
