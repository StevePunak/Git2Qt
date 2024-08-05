#include "tagcollection.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

#include <repository.h>

using namespace GIT;

TagCollection::TagCollection(Repository* repo) :
    GitEntity(TagCollectionEntity, repo)
{
    retrieveTags();
}

Tag TagCollection::createLightweightTag(const QString& name, const GitObject& targetObject)
{
    Tag tag;
    ObjectHandle handle = targetObject.createObjectHandle();
    try
    {
        throwIfTrue(handle.isNull());
        git_oid oid;
        throwOnError(git_tag_create_lightweight(&oid, repository()->handle().value(), name.toUtf8().constData(), handle.value(), false));
        tag = Tag(repository(), name, targetObject.objectId());
    }
    catch(const CommonException&)
    {
    }
    handle.dispose();
    return tag;
}

Tag TagCollection::createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject)
{
    Tag tag;
    ObjectHandle targetHandle = targetObject.createObjectHandle();
    try
    {
        throwIfTrue(targetHandle.isNull());
        Signature sig = signature;      // non-const
        git_oid oid;
        throwOnError(git_tag_annotation_create(&oid, repository()->handle().value(),
                                               name.toUtf8().constData(),
                                               targetHandle.value(),
                                               sig.toNative(),
                                               message.toUtf8().constData()));
        tag = Tag(repository(), name, targetObject.objectId());
    }
    catch(const CommonException&)
    {
    }
    targetHandle.dispose();
    return tag;
}

Tag TagCollection::findTag(const QString& name) const
{
    return _lightweightTags.findByName(name);
}

bool TagCollection::deleteLocalTag(const QString& name)
{
    // TODO

    Tag tag = findTag(name);
    if(tag.isNull()) {

    }
    return false;
}

bool TagCollection::isNull() const
{
    return repository()->isNull();
}

Tag::List TagCollection::retrieveTags()
{
    _lightweightTags.clear();
    try
    {
        throwOnError(git_tag_foreach(repository()->handle().value(), gitTagForeachCallback, this));
    }
    catch(const CommonException&)
    {
    }
    return _lightweightTags;
}

int TagCollection::gitTagForeachCallback(const char* name, git_oid* oid, void* payload)
{
    TagCollection* collection = static_cast<TagCollection*>(payload);
    ObjectId objectId(oid);

    ObjectType type = GitObject::getObjectType(collection->repository(), oid);
    if(type == ObjectTypeTag) {
        collection->_annotatedTags.append(TagAnnotation(collection->repository(), objectId));
    }
    else {
        collection->_lightweightTags.append(Tag(collection->repository(), name, objectId));
    }
    return 0;
}
