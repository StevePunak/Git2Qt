#include "reference.h"

#include <gitexception.h>
#include <referencecollection.h>
#include <repository.h>

#include "log.h"

using namespace GIT;

const QString Reference::LocalBranchPrefix          = "refs/heads/";
const QString Reference::RemoteTrackingBranchPrefix = "refs/remotes/";
const QString Reference::TagPrefix                  = "refs/tags/";
const QString Reference::NotePrefix                 = "refs/notes/";

Reference::Reference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier, ReferenceType referenceType) :
    GitEntity(ReferenceEntity, repo),
    _canonicalName(canonicalName), _targetIdentifier(targetIdentifier),
    _type(referenceType)
{}

Reference::Reference(const Reference& other) :
    GitEntity(ReferenceEntity, other.repository())
{
    *this = other;
}

Reference::~Reference()
{
    if(_target != nullptr) {
        delete _target;
    }
}

Reference& Reference::operator=(const Reference& other)
{
    GitEntity::operator =(other);
    _canonicalName = other._canonicalName;
    _targetIdentifier = other._targetIdentifier;
    _targetOid = other._targetOid;
    _type = other._type;
    _isBranch = other._isBranch;
    _isNote = other._isNote;
    _isRemote = other._isRemote;
    _isTag = other._isTag;

    if(_type == SymbolicReferenceType && repository() != nullptr) {
        resolveTarget();
    }
    return *this;
}

bool Reference::operator ==(const Reference& other) const
{
    bool result = GitEntity::operator ==(other) &&
                  _canonicalName == other._canonicalName &&
                  _targetIdentifier == other._targetIdentifier &&
                  _targetOid == other._targetOid &&
                  _type == other._type &&
                  _isBranch == other._isBranch &&
                  _isNote == other._isNote &&
                  _isRemote == other._isRemote &&
                  _isTag == other._isTag;
    return result;
}

void Reference::resolveProperties()
{
    ReferenceHandle handle = createHandle();
    if(handle.isNull() == false) {
        _isBranch = git_reference_is_branch(handle.value());
        _isNote = git_reference_is_note(handle.value());
        _isRemote = git_reference_is_remote(handle.value());
        _isTag = git_reference_is_tag(handle.value());
    }
}

ReferenceHandle Reference::createHandle() const
{
    ReferenceHandle handle;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repository()->handle().value(), _canonicalName.toUtf8().constData()) == 0) {
        handle = ReferenceHandle(ref);
    }
    return handle;
}

Reference Reference::createSymbolicReferenceObject(Repository* repo, const QString& canonicalName, const QString& targetIdentifier)
{
    Reference reference;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), canonicalName.toUtf8().constData()) == 0) {
        reference = Reference(repo, canonicalName, targetIdentifier, SymbolicReferenceType);
        reference.resolveTarget();
        reference.resolveProperties();
        ReferenceHandle refHandle(ref);
        if(typeFromHandle(refHandle) != SymbolicReferenceType) {
            Log::logText(LVL_ERROR, "Unmatching reference type");
        }
        git_reference_free(ref);
    }
    return reference;
}

Reference Reference::createDirectReferenceObject(Repository* repo, const QString& canonicalName, const ObjectId& targetoid)
{
    Reference reference(repo, canonicalName, targetoid.sha(), DirectReferenceType);
    reference._targetOid = targetoid;
    git_reference* ref = nullptr;
    if(git_reference_lookup(&ref, repo->handle().value(), canonicalName.toUtf8().constData()) == 0) {
        ReferenceHandle refHandle(ref);
        if(typeFromHandle(refHandle) != DirectReferenceType) {
            Log::logText(LVL_ERROR, "Unmatching reference type");
        }
        reference.resolveProperties();

        git_reference_free(ref);
    }
    else {
        Log::logText(LVL_DEBUG, git_error_last()->message);
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
            result = createSymbolicReferenceObject(repo, name, targetIdentifier);
            break;
        }

        case DirectReferenceType:
        {
            ObjectId oid = objectIdFromHandle(handle);
            result = createDirectReferenceObject(repo, name, oid);
            break;
        }

        default:
            break;
        }
    }
    catch(const GitException& e)
    {
        Log::logText(LVL_ERROR, e.message());
    }

    return result;
}

Reference Reference::create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference reference;
    try
    {
        git_reference* ref = nullptr;
        throwOnError(repo, git_reference_create(&ref, repo->handle().value(), name.toUtf8().constData(), targetId.toNative(), allowOverwrite, logMessage.toUtf8().constData()));
        reference = create(repo, ref);
    }
    catch(const GitException& e)
    {}
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
    QString name;
    ReferenceHandle handle = createHandle();
    if(handle.isNull() == false) {
        name = nameFromHandle(handle.value());
        handle.dispose();
    }
    return name;
}

QString Reference::friendlyName() const
{
    QString friendlyName = _canonicalName;
    if(looksLikeLocalBranch()) {
        friendlyName = _canonicalName.mid(LocalBranchPrefix.length());
    }
    else if(looksLikeRemoteTrackingBranch()) {
        friendlyName = _canonicalName.mid(RemoteTrackingBranchPrefix.length());
    }
    else if(looksLikeNote()) {
        friendlyName = _canonicalName.mid(NotePrefix.length());
    }
    else if(looksLikeTag()) {
        friendlyName = _canonicalName.mid(TagPrefix.length());
    }
    return friendlyName;
}

#if 0
ReferenceType Reference::type() const
{
    ReferenceType result = UnknownReferenceType;
    ReferenceHandle handle = createHandle();
    if(handle.isNull() == false) {
        result = typeFromHandle(handle.value());
        handle.dispose();
    }
    return result;
}
#endif

ObjectId Reference::objectId() const
{
    ObjectId result;
    ReferenceHandle handle = createHandle();
    if(handle.isNull() == false) {
        result = objectIdFromHandle(handle);
        handle.dispose();
    }
    return result;
}

void Reference::resolveTarget()
{
    if(_target != nullptr) {
        delete _target;
        _target = nullptr;
    }

    try
    {
        git_reference* targetRef = nullptr;
        ReferenceHandle handle = createHandle();
        throwOnError(git_reference_resolve(&targetRef, handle.value()));
        const git_oid* oid = git_reference_target(targetRef);
        if(oid != nullptr) {
            _targetOid = ObjectId(oid);
            _target = new Reference(repository(), _targetIdentifier, _targetOid.toString(), DirectReferenceType);
            handle.dispose();
        }
    }
    catch(const GitException&)
    {
    }
}

bool Reference::isNull() const
{
    bool result = true;
    if(repository() != nullptr) {
        ReferenceHandle handle = createHandle();
        result = handle.isNull();
        handle.dispose();
    }
    return result;
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

