#include "objectid.h"

#include <reference.h>

using namespace GIT;

ObjectId::ObjectId(const GitOid& oid) :
    _oid(oid)
{
    _sha = oid.id().toHex().toLower();
}

ObjectId::ObjectId(const git_oid& oid) :
    _oid(oid)
{
    _sha = _oid.id().toHex().toLower();
}

ObjectId::ObjectId(const git_oid* oid) :
    _oid(oid)
{
    _sha = _oid.id().toHex().toLower();
}

ObjectId::ObjectId(const git_object* obj)
{
    const git_oid* oid = git_object_id(obj);
    if(oid != nullptr) {
        _oid = GitOid(oid);
        _sha = _oid.id().toHex().toLower();
    }
}

ObjectId ObjectId::createFromHandle(git_reference* handle)
{
    ObjectId result;
    const git_oid* oid = git_reference_target(handle);
    if(oid != nullptr) {
        GitOid gitOid(oid->id, GIT_OID_MAX_SIZE);
        result = ObjectId(gitOid);
    }
    return result;
}

ObjectId ObjectId::createFromReference(const Reference& reference)
{
    ObjectId result;
    ReferenceHandle referenceHandle = reference.createHandle();
    if(referenceHandle.isNull() == false) {
        result = createFromHandle(referenceHandle.value());
        referenceHandle.dispose();
    }
    return result;
}
