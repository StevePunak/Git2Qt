/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of references. It is used by the repository
 * object to maintain the repo references.
 *
 * TODO: Should probably not be a public header.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REFERENCECOLLECTION_H
#define REFERENCECOLLECTION_H
#include <git2qt/gitentity.h>
#include <git2qt/objectid.h>
#include <git2qt/reference.h>

namespace GIT {

class Repository;
class ReferenceCollection : public GitEntity
{
public:
    ReferenceCollection(Repository* repo);
    virtual ~ReferenceCollection();

    void resolveSymbolicTargets();
    Reference head();
    Reference findReference(const QString& name) const;
    Reference findReference(const ObjectId& objectId) const;
    void clear();

    void appendDirectReference(const Reference& reference);
    void appendDirectReference(const QList<Reference>& references);
    Reference appendDirectReference(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite = false);

    Reference::Map references() const { return _references; }

    Reference updateTarget(const Reference& directRef, const ObjectId& targetId, const QString& logMessage);
    Reference updateHeadTarget(const ObjectId& targetId, const QString& logMessage);
    Reference updateDirectReferenceTarget(const Reference& directRef, const ObjectId& targetId, const QString& logMessage);

    virtual bool isNull() const { return false; }

private:
    Reference _head;
    Reference::Map _references;
};

} // namespace GIT

#endif // REFERENCECOLLECTION_H
