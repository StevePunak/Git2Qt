#ifndef REFERENCECOLLECTION_H
#define REFERENCECOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/reference.h>

namespace GIT {

class Repository;
class ReferenceCollection : public GitEntity
{
public:
    ReferenceCollection(Repository* repo);
    virtual ~ReferenceCollection();

    void resolveSymbolicTargets(Repository* repo);
    Reference* head();
    Reference* findReference(const QString& name) const;
    Reference* findReference(const ObjectId& objectId) const;
    void clear();

    void append(Reference* reference);
    void append(QList<Reference*> references);
    DirectReference* append(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite = false);

    Reference* updateTarget(DirectReference* directRef, const ObjectId& targetId, const QString& logMessage);
    Reference* updateHeadTarget(const ObjectId& targetId, const QString& logMessage);
    Reference* updateDirectReferenceTarget(DirectReference* directRef, const ObjectId& targetId, const QString& logMessage);

    virtual bool isNull() const { return false; }

private:
    Reference::Map _references;
    Reference* _head = nullptr;
};

} // namespace GIT

#endif // REFERENCECOLLECTION_H
