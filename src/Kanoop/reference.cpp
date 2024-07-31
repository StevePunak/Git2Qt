#include "reference.h"

#include <referencecollection.h>
#include <repository.h>

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

using namespace GIT;

const QString Reference::LocalBranchPrefix          = "refs/heads/";
const QString Reference::RemoteTrackingBranchPrefix = "refs/remotes/";
const QString Reference::TagPrefix                  = "refs/tags/";
const QString Reference::NotePrefix                 = "refs/notes/";

Reference::Reference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier) :
    GitEntity(ReferenceEntity, repo),
    _canonicalName(canonicalName), _targetIdentifier(targetIdentifier)
{}

Reference::~Reference()
{
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
    if(git_reference_create(&ref, repo->handle().value(), name.toUtf8().constData(), targetId.toNative(), allowOverwrite, logMessage.toUtf8().constData()) == 0) {
        result = create(repo, ref);
    }
    return result;
}

Reference* Reference::lookup(Repository* repo, const QString& name)
{
    Reference* result = nullptr;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), name.toUtf8()) == 0) {
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

ObjectId Reference::objectId() const
{
    return objectIdFromHandle(_reference);
}

Reference::ReferenceType Reference::typeFromHandle(const ReferenceHandle& handle)
{
    ReferenceType result = UnknownReferenceType;

    git_reference_t type = git_reference_type(handle.value());
    if(type == GIT_REFERENCE_DIRECT) {
        result = DirectReferenceType;
    }
    else if(type == GIT_REFERENCE_SYMBOLIC) {
        result = SymbolicReferenceType;
    }
    return result;
}

QString Reference::nameFromHandle(const ReferenceHandle& handle)
{
    const char* name = git_reference_name(handle.value());
    return name;
}

QString Reference::symbolicTargetNameFromHandle(const ReferenceHandle& handle)
{
    const char* name = git_reference_symbolic_target(handle.value());
    return name;
}

ObjectId Reference::objectIdFromHandle(const ReferenceHandle& handle)
{
    ObjectId result;
    const git_oid* oid = git_reference_target(handle.value());
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


