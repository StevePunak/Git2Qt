#include "tagcollection.h"

#include <Kanoop/commonexception.h>

#include <repository.h>

using namespace GIT;

TagCollection::TagCollection(Repository* repo) :
    GitEntity(TagCollectionEntity, repo) {}

Tag::List TagCollection::retrieveTags()
{
    Tag::List list;
    _retrieveTags.clear();
    try
    {
        throwOnError(git_tag_foreach(repository()->handle().value(), gitTagForeachCallback, this));
        list = _retrieveTags;
        _retrieveTags.clear();
    }
    catch(const CommonException&)
    {
    }
    return list;
}

bool TagCollection::isNull() const
{
    return repository()->isNull();
}

int TagCollection::gitTagForeachCallback(const char* name, git_oid* oid, void* payload)
{
    TagCollection* collection = static_cast<TagCollection*>(payload);
    collection->_retrieveTags.append(Tag(collection->repository(), name, ObjectId(oid)));
    return 0;
}
