#include "tagcollection.h"

#include <annotatedtag.h>
#include <gitexception.h>
#include <lightweighttag.h>
#include <repository.h>

using namespace GIT;

TagCollection::TagCollection(Repository* repo) :
    GitEntity(TagCollectionEntity, repo)
{
    refreshTags();
}

const LightweightTag* TagCollection::createLightweightTag(const QString& name, const GitObject& targetObject)
{
    LightweightTag* tag = nullptr;
    ObjectHandle handle = targetObject.createObjectHandle();
    try
    {
        throwIfTrue(handle.isNull());
        git_oid oid;
        throwOnError(git_tag_create_lightweight(&oid, repository()->handle().value(), name.toUtf8().constData(), handle.value(), false));
        tag = new LightweightTag(repository(), name, targetObject.objectId());
        _tags.append(tag);
    }
    catch(const GitException&)
    {
    }
    handle.dispose();
    return tag;
}

const AnnotatedTag* TagCollection::createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject, bool allowOverwrite)
{
    AnnotatedTag* tag = nullptr;
    ObjectHandle targetHandle = targetObject.createObjectHandle();
    try
    {
        throwIfTrue(targetHandle.isNull());
        Signature sig = signature;      // non-const
        git_oid oid;
        throwOnError(git_tag_create(&oid,
                                    repository()->handle().value(),
                                    name.toUtf8().constData(),
                                    targetHandle.value(),
                                    sig.toNative(),
                                    message.toUtf8().constData(),
                                    allowOverwrite));
        tag = new AnnotatedTag(repository(), name, ObjectId(oid));
        _tags.append(tag);
    }
    catch(const GitException&)
    {
    }
    targetHandle.dispose();
    return tag;
}

const Tag* TagCollection::findTag(const QString& name) const
{
    return _tags.findByName(name);
}

const Tag* TagCollection::findTag(const ObjectId& objectId) const
{
    return _tags.findByObjectId(objectId);
}

bool TagCollection::deleteLocalTag(const QString& name)
{
    bool result = false;
    try
    {
        throwOnError(git_tag_delete(repository()->handle().value(), name.toUtf8().constData()));
        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }

    return result;
}

bool TagCollection::isNull() const
{
    return repository()->isNull();
}

void TagCollection::reload()
{
    refreshTags();
}

void TagCollection::refreshTags()
{
    qDeleteAll(_tags);
    _tags.clear();
    _tagsByCommit.clear();
    try
    {
        throwOnError(git_tag_foreach(repository()->handle().value(), gitTagForeachCallback, this));
    }
    catch(const GitException&)
    {
    }
}

int TagCollection::gitTagForeachCallback(const char* name, git_oid* oid, void* payload)
{
    TagCollection* collection = static_cast<TagCollection*>(payload);
    ObjectId objectId(oid);

    ObjectType type = GitObject::getObjectType(collection->repository(), oid);
    Tag* tag;
    if(type == ObjectTypeTag) {
        tag = new AnnotatedTag(collection->repository(), name, objectId);
        collection->_tags.append(tag);
        collection->_tagsByCommit[tag->targetObjectId()].append(tag);
    }
    else {
        tag = new LightweightTag(collection->repository(), name, objectId);
        collection->_tags.append(tag);
        collection->_tagsByCommit[objectId].append(tag);
    }
    return 0;
}
