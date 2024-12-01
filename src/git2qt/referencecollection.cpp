#include "referencecollection.h"

#include <gitexception.h>
#include <repository.h>

#include "log.h"

#include <git2qt/private/historyrewriter.h>

using namespace GIT;

ReferenceCollection::ReferenceCollection(Repository* repo) :
    GitEntity(ReferenceCollectionEntity, repo)
{
}

ReferenceCollection::~ReferenceCollection()
{
    for(Reference& reference : _references) {
        Q_UNUSED(reference)
        // reference.dispose();
    }
}

void ReferenceCollection::resolveSymbolicTargets()
{
    for(Reference& reference : _references) {
        if(reference.type() != SymbolicReferenceType) {
            continue;
        }
        if(reference.target().isNull()) {
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

void ReferenceCollection::appendDirectReferences(const QList<Reference>& references)
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

bool ReferenceCollection::rewriteHistory(RewriteHistoryOptions* options, const Commit::List& commits)
{
    if(_references.count() == 0) {
        return false;
    }

    HistoryRewriter rewriter(repository(), commits, options);
    bool result = rewriter.execute();
    return result;
}

Reference ReferenceCollection::appendDirectReference(const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite)
{
    Reference reference = Reference::create(repository(), name, targetId, logMessage, allowOverwrite);
    if(reference.isNull() || reference.type() != DirectReferenceType) {
        Log::logText(LVL_ERROR, "Failed to create DIRECT reference");
    }
    return reference;
}

bool ReferenceCollection::deleteLocalReference(const Reference& reference)
{
    bool result = false;

    ReferenceHandle handle = reference.createHandle();
    try
    {
        throwIfTrue(handle.isNull(), "Invalid reference");
        throwOnError(git_reference_delete(handle.value()));
        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

ReferenceList ReferenceCollection::findReachableFrom(const ReferenceList& subset, const Commit::List& commits) const
{
    ReferenceList result;

    ReferenceList refs = subset;
    if(refs.count() == 0) {
        return result;
    }

    ObjectId::List targetsSet = commits.objectIds().unique();
    if(targetsSet.isEmpty()) {
        return result;
    }

    for(const Reference& reference : refs) {
        Reference directRef = reference.resolveToDirectReference();
        if(directRef.isNull()) {
            continue;
        }

        Commit commit = directRef.peelToCommit();
        if(commit.isNull()) {
            continue;
        }

        ObjectId commitId = commit.objectId();
        for(const ObjectId& potentialAncestorId : targetsSet) {
            if(potentialAncestorId == commitId) {
                result.append(reference);
                break;
            }

            if(git_graph_descendant_of(repository()->handle().value(), commitId.toNative(), potentialAncestorId.toNative())) {
                result.append(reference);
                break;
            }
        }
    }

    return result;
}

ReferenceList ReferenceCollection::findReachableFrom(const Commit::List& commits) const
{
    ReferenceList refs = _references.values();
    return findReachableFrom(refs, commits);
}
