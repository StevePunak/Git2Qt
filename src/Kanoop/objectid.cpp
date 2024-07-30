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
    _oid(*oid)
{
    _sha = _oid.id().toHex().toLower();
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

ObjectId ObjectId::createFromReference(Reference* reference)
{
    return createFromHandle(reference->handle());
}
