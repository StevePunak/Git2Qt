#include "gitoid.h"

using namespace GIT;

GitOid::GitOid()
{
}

GitOid::GitOid(const unsigned char* data, int size)
{
    memcpy(_id.id, data, qMin(size, GIT_OID_MAX_SIZE));
}

GitOid::GitOid(const git_oid& oid)
{
    _id = oid;
}
