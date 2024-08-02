#include "referencecollection.h"

#include <repository.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

using namespace GIT;

ReferenceCollection::ReferenceCollection(Repository* repo) :
    GitEntity(ReferenceCollectionEntity, repo)
{
}

ReferenceCollection::~ReferenceCollection()
{
}

void ReferenceCollection::resolveSymbolicTargets()
{
    for(Reference& reference : _references) {
        if(reference.type() != SymbolicReferenceType) {
            continue;
        }
        if(reference.target() == nullptr) {
            reference.resolveTarget();  // setTarget(repo->references()->findReference(symbolicReference->targetIdentifier()));
        }
    }
}

Reference ReferenceCollection::head()
{
    try
    {
        Reference tmpRef;
        throwIfTrue((tmpRef = Reference::lookup(repository(), "HEAD")).isNull());
        if(_head.isNull() == false) {
            if(_head.isSymbolic() == false || tmpRef.isSymbolic() == false) {
                throw CommonException("HEAD ref not symbolic");
            }
            // If they are not the same, replace head with new
            // Otherwise, continue to use the old head ref
            if(_head.targetIdentifier() != tmpRef.targetIdentifier()) {
                _head = tmpRef;
            }
        }
        else {
            _head = tmpRef;
        }
    }
    catch(const CommonException&)
    {
    }
    return _head;
}

Reference ReferenceCollection::findReference(const QString& name) const
{
    return _references.value(name);
}

Reference ReferenceCollection::findReference(const ObjectId& objectId) const
{
    return _references.findByObjectId(objectId);
}

void ReferenceCollection::clear()
{
    _references.clear();
}

void ReferenceCollection::append(const Reference& reference)
{
    _references.insert(reference.name(), reference);
}

void ReferenceCollection::append(const QList<Reference>& references)
{
    for(const Reference& reference : references) {
        append(reference);
    }
}

Reference ReferenceCollection::updateTarget(const Reference& directRef, const ObjectId& targetId, const QString& logMessage)
{
    Reference reference;
    if(directRef.canonicalName() == "HEAD") {
        reference = updateHeadTarget(targetId, logMessage);
    }
    else {
        reference = updateDirectReferenceTarget(directRef, targetId, logMessage);
    }
    return reference;
}

Reference ReferenceCollection::updateHeadTarget(const ObjectId& targetId, const QString& logMessage)
{
    return append("HEAD", targetId, logMessage, true);
}

Reference ReferenceCollection::updateDirectReferenceTarget(const Reference& directRef, const ObjectId& targetId, const QString& logMessage)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_set_target(&ref, directRef.handle().value(), targetId.toNative(), logMessage.toUtf8().constData()) == 0) {
        reference = Reference::create(repository(), ref);
        append(reference);
    }
    return reference;
}

Reference ReferenceCollection::append(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference reference = Reference::create(repository(), name, targetId, logMessage, allowOverwrite);
    if(reference.isNull() || reference.type() != DirectReferenceType) {
        KLog::sysLogText(KLOG_ERROR, "Failed to create DIRECT reference");
    }
    return reference;
}
