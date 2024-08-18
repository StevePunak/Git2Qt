#include "tagcollection.h"

#include <Kanoop/klog.h>

#include <annotatedtag.h>
#include <gitexception.h>
#include <lightweighttag.h>
#include <repository.h>

using namespace GIT;

TagCollection::TagCollection(Repository* repo) :
    GitEntity(TagCollectionEntity, repo)
{
    retrieveTags();
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
    }
    catch(const GitException&)
    {
    }
    handle.dispose();
    return tag;
}

const AnnotatedTag* TagCollection::createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject)
{
    AnnotatedTag* tag = nullptr;
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
        tag = new AnnotatedTag(repository(), name, targetObject.objectId());
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

bool TagCollection::deleteLocalTag(const QString& name)
{
    // TODO

    Tag* tag = _tags.findByName(name);
    if(tag->isNull()) {

    }
    return false;
}

bool TagCollection::isNull() const
{
    return repository()->isNull();
}

void TagCollection::reload()
{
    retrieveTags();
}

void TagCollection::retrieveTags()
{
    qDeleteAll(_tags);
    _tags.clear();
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
    if(type == ObjectTypeTag) {
        collection->_tags.append(new AnnotatedTag(collection->repository(), name, objectId));
    }
    else {
        collection->_tags.append(new LightweightTag(collection->repository(), name, objectId));
    }
    return 0;
}
