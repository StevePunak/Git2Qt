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
    if(_target == nullptr) {
        delete _target;
    }
}

Reference Reference::createSymbolicReference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier, git_reference* handle)
{
    Reference reference(repo, canonicalName, targetIdentifier);
    reference._target = new Reference(repo, canonicalName, targetIdentifier);
    reference._handle = ReferenceHandle(handle);
    return reference;
}

Reference Reference::createSymbolicReference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), canonicalName.toUtf8().constData()) == 0) {
        reference = createSymbolicReference(repo, canonicalName, targetIdentifier, ref);
    }
    return reference;
}

Reference Reference::createDirectReference(Repository* repo, const QString& canonicalName, const ObjectId& targetoid, git_reference* handle)
{
    Reference reference(repo, canonicalName, targetoid.sha());
    reference._targetOid = targetoid;
    reference._handle = ReferenceHandle(handle);
    return reference;
}

Reference Reference::createDirectReference(Repository* repo, const QString& canonicalName, const ObjectId& targetoid)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), canonicalName.toUtf8().constData()) == 0) {
        reference = createDirectReference(repo, canonicalName, targetoid, ref);
    }
    return reference;
}

Reference Reference::create(Repository* repo, git_reference* handle)
{
    Reference result;

    try
    {
        ReferenceType type = typeFromHandle(handle);
        QString name = nameFromHandle(handle);

        switch(type) {
        case SymbolicReferenceType:
        {
            QString targetIdentifier = symbolicTargetNameFromHandle(handle);
            result = createSymbolicReference(repo, name, targetIdentifier, handle);
            break;
        }

        case DirectReferenceType:
        {
            ObjectId oid = objectIdFromHandle(handle);
            result = createDirectReference(repo, name, oid, handle);
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

Reference Reference::create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_create(&ref, repo->handle().value(), name.toUtf8().constData(), targetId.toNative(), allowOverwrite, logMessage.toUtf8().constData()) == 0) {
        reference = create(repo, ref);
    }
    return reference;
}

Reference Reference::lookup(Repository* repo, const QString& name)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), name.toUtf8()) == 0) {
        reference = create(repo, ref);
    }
    return reference;
}

QString Reference::name() const
{
    return nameFromHandle(_handle);
}

ReferenceType Reference::type() const
{
    return typeFromHandle(_handle);
}

ObjectId Reference::objectId() const
{
    return objectIdFromHandle(_handle);
}

void Reference::resolveTarget()
{
    if(_target != nullptr) {
        delete _target;
        _target = nullptr;
    }
    _target = new Reference(repository(), _canonicalName, _targetIdentifier);
}

ReferenceType Reference::typeFromHandle(const ReferenceHandle& handle)
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

