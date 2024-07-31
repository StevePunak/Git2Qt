#ifndef OBJECTID_H
#define OBJECTID_H
#include <Kanoop/git/gitoid.h>
#include <git2.h>
#include <QString>
#include <QList>

namespace GIT {

class Reference;

class ObjectId
{
public:
    ObjectId() {}
    ObjectId(const GitOid& oid);
    ObjectId(const git_oid& oid);
    ObjectId(const git_oid* oid);

    bool operator ==(const ObjectId& other) const { return _sha == other._sha; }
    bool operator !=(const ObjectId& other) const { return !(*this == other); }

    static ObjectId createFromHandle(git_reference* handle);
    static ObjectId createFromReference(Reference* reference);

    QString sha() const { return _sha; }
    GitOid oid() const { return _oid; }

    const git_oid* toNative() const { return _oid.toNative(); }

    bool isValid() const { return _oid.isEmtpy() == false; }

    class List : public QList<ObjectId> {};

private:
    GitOid _oid;
    QString _sha;

    static const int _rawSize = GitOid::Size;

public:
    static const int HexSize = GitOid::Size * 2;
};

} // namespace GIT

#endif // OBJECTID_H
