/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Wrapper for git_oid from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GITOID_H
#define GITOID_H

#include <QByteArray>
#include <git2.h>
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT GitOid
{
public:
    GitOid();
    GitOid(const unsigned char* data, int size);
    GitOid(const git_oid& oid);
    GitOid(const git_oid* oid);
    GitOid(const QString& sha);

    QByteArray id() const { return QByteArray((const char*)_id.id, GIT_OID_MAX_SIZE); }

    const git_oid* toNative() const { return &_id; }

    bool isEmtpy() const;

private:
    git_oid _id = { 0 };

    static const git_oid Empty;

public:
    static const int Size = 20;
};

} // namespace GIT

#endif // GITOID_H
