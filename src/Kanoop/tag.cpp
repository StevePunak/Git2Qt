#include "tag.h"

using namespace GIT;

Tag::Tag(Repository* repo, const QString& name, const ObjectId& objectId) :
    GitEntity(TagEntity, repo),
    _name(name), _objectId(objectId) {}
