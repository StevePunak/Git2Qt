#include "gitoid.h"

using namespace GIT;

const git_oid GitOid::Empty = { 0 };

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

bool GitOid::isEmtpy() const
{
    return memcmp(Empty.id, _id.id, GIT_OID_MAX_SIZE) == 0;
}
