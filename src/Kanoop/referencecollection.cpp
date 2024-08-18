#include "referencecollection.h"

#include <gitexception.h>
#include <repository.h>

#include "log.h"

using namespace GIT;

ReferenceCollection::ReferenceCollection(Repository* repo) :
    GitEntity(ReferenceCollectionEntity, repo)
{
}

ReferenceCollection::~ReferenceCollection()
{
    for(Reference& reference : _references) {
        reference.dispose();
    }
}

void ReferenceCollection::resolveSymbolicTargets()
{
    for(Reference& reference : _references) {
        if(reference.type() != SymbolicReferenceType) {
            continue;
        }
        if(reference.target() == nullptr) {
            reference.resolveTarget();
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
                throw GitException("HEAD ref not symbolic");
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
        _head.resolveTarget();
    }
    catch(const GitException&)
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

void ReferenceCollection::appendDirectReference(const Reference& reference)
{
    _references.insert(reference.name(), reference);
}

void ReferenceCollection::appendDirectReference(const QList<Reference>& references)
{
    for(const Reference& reference : references) {
        appendDirectReference(reference);
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
    return appendDirectReference("HEAD", targetId, logMessage, true);
}

Reference ReferenceCollection::updateDirectReferenceTarget(const Reference& directRef, const ObjectId& targetId, const QString& logMessage)
{
    Reference reference;
    git_reference* ref = nullptr;

    ReferenceHandle referenceHandle = directRef.createHandle();
    if(referenceHandle.isNull() == false) {
        if(git_reference_set_target(&ref, referenceHandle.value(), targetId.toNative(), logMessage.toUtf8().constData()) == 0) {
            reference = Reference::create(repository(), ref);
            appendDirectReference(reference);
        }
        referenceHandle.dispose();
    }
    return reference;
}

Reference ReferenceCollection::appendDirectReference(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference reference = Reference::create(repository(), name, targetId, logMessage, allowOverwrite);
    if(reference.isNull() || reference.type() != DirectReferenceType) {
        Log::sysLogText(KLOG_ERROR, "Failed to create DIRECT reference");
    }
    return reference;
}
