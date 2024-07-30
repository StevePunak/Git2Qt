#ifndef GITOID_H
#define GITOID_H

#include <QByteArray>
#include <git2.h>

namespace GIT {

class GitOid
{
public:
    GitOid();
    GitOid(const unsigned char* data, int size);
    GitOid(const git_oid& oid);

    QByteArray id() const { return QByteArray((const char*)_id.id, GIT_OID_MAX_SIZE); }

    const git_oid* toNative() const { return &_id; }

private:
    git_oid _id;

public:
    static const int Size = 20;
};

} // namespace GIT

#endif // GITOID_H
