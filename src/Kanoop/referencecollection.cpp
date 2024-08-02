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
    qDeleteAll(_references);
    if(_head != nullptr) {
        delete _head;
    }
}

void ReferenceCollection::resolveSymbolicTargets(Repository* repo)
{
    for(Reference* reference : _references) {
        SymbolicReference* symbolicReference = dynamic_cast<SymbolicReference*>(reference);
        if(symbolicReference == nullptr) {
            continue;
        }
        if(symbolicReference->target() == nullptr) {
            symbolicReference->setTarget(repo->references()->findReference(symbolicReference->targetIdentifier()));
        }
    }
}

Reference* ReferenceCollection::head()
{
    try
    {
        Reference* tmpRef = nullptr;
        throwIfNull((tmpRef = Reference::lookup(repository(), "HEAD")));
        if(_head != nullptr) {
            if(_head->isSymbolic() == false || tmpRef->isSymbolic() == false) {
                throw CommonException("HEAD ref not symbolic");
            }
            // If they are not the same, replace head with new
            // Otherwise, continue to use the old head ref
            if(static_cast<SymbolicReference*>(_head)->targetIdentifier() != static_cast<SymbolicReference*>(tmpRef)->targetIdentifier()) {
                delete _head;
                _head = tmpRef;
            }
            else {
                delete tmpRef;
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

Reference* ReferenceCollection::findReference(const QString& name) const
{
    return _references.value(name);
}

Reference* ReferenceCollection::findReference(const ObjectId& objectId) const
{
    return _references.findByObjectId(objectId);
}

void ReferenceCollection::clear()
{
    qDeleteAll(_references);
    _references.clear();
}

void ReferenceCollection::append(Reference* reference)
{
    _references.insert(reference->name(), reference);
}

void ReferenceCollection::append(QList<Reference*> references)
{
    for(Reference* reference : references) {
        append(reference);
    }
}

Reference* ReferenceCollection::updateTarget(DirectReference* directRef, const ObjectId& targetId, const QString& logMessage)
{
    Reference* result = nullptr;
    if(directRef->canonicalName() == "HEAD") {
        result = updateHeadTarget(targetId, logMessage);
    }
    else {
        result = updateDirectReferenceTarget(directRef, targetId, logMessage);
    }
    return result;
}

Reference* ReferenceCollection::updateHeadTarget(const ObjectId& targetId, const QString& logMessage)
{
    return append("HEAD", targetId, logMessage, true);
}

Reference* ReferenceCollection::updateDirectReferenceTarget(DirectReference* directRef, const ObjectId& targetId, const QString& logMessage)
{
    Reference* result = nullptr;
    git_reference* ref = nullptr;
    if(git_reference_set_target(&ref, directRef->handle(), targetId.toNative(), logMessage.toUtf8().constData()) == 0) {
        result = Reference::create(repository(), ref);
        append(result);
    }
    return result;
}

DirectReference* ReferenceCollection::append(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference* reference = Reference::create(repository(), name, targetId, logMessage, allowOverwrite);
    if(reference == nullptr || reference->type() != DirectReferenceType) {
        KLog::sysLogText(KLOG_ERROR, "Failed to create DIRECT reference");
    }
    return static_cast<DirectReference*>(reference);
}
