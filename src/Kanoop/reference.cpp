#include "reference.h"

#include <referencecollection.h>
#include <repository.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

using namespace GIT;

Reference::Reference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier) :
    GitEntity(ReferenceEntity, repo),
    _canonicalName(canonicalName), _targetIdentifier(targetIdentifier)
{}

Reference::~Reference()
{
    if(_reference != nullptr) {
        git_reference_free(_reference);
    }
}

Reference* Reference::create(Repository* repo, git_reference* handle)
{
    Reference* result = nullptr;

    try
    {
        ReferenceType type = typeFromHandle(handle);
        QString name = nameFromHandle(handle);

        switch(type) {
        case SymbolicReferenceType:
        {
            QString targetIdentifier = symbolicTargetNameFromHandle(handle);
            Reference* targetRef = repo->references()->findReference(targetIdentifier);
            result = new SymbolicReference(repo, name, targetIdentifier, targetRef);
            result->_reference = handle;
            break;
        }

        case DirectReferenceType:
        {
            ObjectId oid = objectIdFromHandle(handle);
            result = new DirectReference(repo, name, oid);
            result->_reference = handle;
            break;
        }

        default:
            break;
        }
    }
    catch(const CommonException& e)
    {
        KLog::sysLogText(KLOG_ERROR, e.message());
    }

    return result;
}

Reference* Reference::create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference* result = nullptr;
    git_reference* ref = nullptr;
    if(git_reference_create(&ref, repo->handle(), name.toUtf8().constData(), targetId.toNative(), allowOverwrite, logMessage.toUtf8().constData()) == 0) {
        result = create(repo, ref);
    }
    return result;
}

Reference* Reference::lookup(Repository* repo, const QString& name)
{
    Reference* result = nullptr;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle(), name.toUtf8()) == 0) {
        result = create(repo, ref);
    }
    return result;
}

QString Reference::name() const
{
    return nameFromHandle(_reference);
}

Reference::ReferenceType Reference::type() const
{
    return typeFromHandle(_reference);
}

Reference::ReferenceType Reference::typeFromHandle(git_reference* handle)
{
    ReferenceType result = UnknownReferenceType;

    git_reference_t type = git_reference_type(handle);
    if(type == GIT_REFERENCE_DIRECT) {
        result = DirectReferenceType;
    }
    else if(type == GIT_REFERENCE_SYMBOLIC) {
        result = SymbolicReferenceType;
    }
    return result;
}

QString Reference::nameFromHandle(git_reference* handle)
{
    const char* name = git_reference_name(handle);
    return name;
}

QString Reference::symbolicTargetNameFromHandle(git_reference* handle)
{
    const char* name = git_reference_symbolic_target(handle);
    return name;
}

ObjectId Reference::objectIdFromHandle(git_reference* handle)
{
    ObjectId result;
    const git_oid* oid = git_reference_target(handle);
    if(oid != nullptr) {
        GitOid gitOid(oid->id, GIT_OID_MAX_SIZE);
        result = ObjectId(gitOid);
    }
    return result;
}

// --------------------- SymbolicReference ----------------------

SymbolicReference::SymbolicReference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier, Reference* target) :
    Reference(repo, canonicalName, targetIdentifier),
    _target(target)
{
}

// --------------------- DirectReference ----------------------

DirectReference::DirectReference(Repository* repo, const QString& canonicalName, const ObjectId& targetOid) :
    Reference(repo, canonicalName, targetOid.sha()),
    _targetOid(targetOid)
{
}


