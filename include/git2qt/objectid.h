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
#include <git2qt/declspec.h>

namespace GIT {

class Reference;

class GIT2QT_EXPORT ObjectId
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

    uint64_t p1() const { return _p1; }
    uint64_t p2() const { return _p2; }
    uint64_t p3() const { return _p3; }

    const git_oid* toNative() const { return _oid.toNative(); }

    QString toString(int count = 0) const { return count == 0 ? _sha : _sha.left(count); }
    bool isValid() const { return _oid.isEmtpy() == false; }
    bool isNull() const { return !isValid(); }

    QVariant toVariant() const { return QVariant::fromValue<ObjectId>(*this); }
    static ObjectId fromVariant(const QVariant& value) { return value.value<ObjectId>(); }

    static bool isValid(const QString& sha);

    class List : public QList<ObjectId>
    {
    public:
        List() {}
        List(const QList<ObjectId>& other) {
            for(const ObjectId& o : other) {
                append(o);
            }
        }

        List unique() const
        {
            List result;
            for(const ObjectId& objectId : *this) {
                if(result.contains(objectId) == false) {
                    result.append(objectId);
                }
            }
            return result;
        }
    };

    /**
     * @brief The Set class
     * Use this for fast lookups
     */
    class Set : public QSet<ObjectId>
    {
    public:
        Set() {}
        Set(const QList<ObjectId>& other) {
            for(const ObjectId& o : other) {
                insert(o);
            }
        }

        bool containsAll(const ObjectId::List& other) const
        {
            int found = 0;
            for(const ObjectId& o : other) {
                if(contains(o)) {
                    found++;
                }
            }
            return found >= other.count();
        }
    };

private:
    void calculateParts();

    GitOid _oid;
    QString _sha;

    uint64_t _p1 = 0;
    uint64_t _p2 = 0;
    uint64_t _p3 = 0;

    static const int _rawSize = GitOid::Size;

public:
    static const int HexSize = GitOid::Size * 2;
};

} // namespace GIT

namespace std {

template <> struct hash<GIT::ObjectId>
{
    size_t operator()(const GIT::ObjectId &key, size_t seed) const
    {
        return qHashMulti(seed, key.p1(), key.p2(), key.p3());
    }
};

}

Q_DECLARE_METATYPE(GIT::ObjectId)

#endif // OBJECTID_H
