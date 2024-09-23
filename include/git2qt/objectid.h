/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a SHA objectId. It wraps and extends the GitOid class
 * which in turn directly wraps git_oit from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef OBJECTID_H
#define OBJECTID_H
#include <git2qt/gitoid.h>
#include <git2.h>
#include <QString>
#include <QList>
#include <QVariant>

namespace GIT {

class Reference;

class ObjectId
{
public:
    ObjectId() {}
    ObjectId(const GitOid& oid);
    ObjectId(const git_oid& oid);
    ObjectId(const git_oid* oid);
    ObjectId(const git_object* obj);
    ObjectId(const QString& sha);

    bool operator ==(const ObjectId& other) const { return _sha == other._sha; }
    bool operator !=(const ObjectId& other) const { return !(*this == other); }
    bool operator <(const ObjectId& other) const { return _sha < other._sha; }
    bool operator >(const ObjectId& other) const { return _sha > other._sha; }

    bool operator ==(const QString& sha) const { return _sha == sha; }
    bool operator !=(const QString& other) const { return !(*this == other); }

    static ObjectId createFromHandle(git_reference* handle);
    static ObjectId createFromReference(const Reference& reference);

    QString sha() const { return _sha; }
    GitOid oid() const { return _oid; }

    const git_oid* toNative() const { return _oid.toNative(); }

    QString toString(int count = 0) const { return count == 0 ? _sha : _sha.left(count); }
    bool isValid() const { return _oid.isEmtpy() == false; }
    bool isNull() const { return !isValid(); }

    QVariant toVariant() const { return QVariant::fromValue<ObjectId>(*this); }
    static ObjectId fromVariant(const QVariant& value) { return value.value<ObjectId>(); }

    class List : public QList<ObjectId> {};

private:
    GitOid _oid;
    QString _sha;

    static const int _rawSize = GitOid::Size;

public:
    static const int HexSize = GitOid::Size * 2;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::ObjectId)

#endif // OBJECTID_H
