#include "tag.h"
#include "repository.h"

#include "log.h"

#include <annotatedtag.h>
#include <blob.h>
#include <lightweighttag.h>
#include <tree.h>

using namespace GIT;

Tag::Tag() :
    GitObject(TagLightweightEntity, nullptr, ObjectId()) {}

Tag::Tag(GitEntityType entityType, Repository* repo, const QString& name, const ObjectId& objectId) :
    GitObject(entityType, repo, objectId),
    _name(name)
{
}

Tag::Tag(const Tag& other) :
    GitObject(other.entityType(), repository(), other.objectId())
{
    *this = other;
}

Tag& Tag::operator=(const Tag& other)
{
    GitObject::operator =(other);
    _name = other._name;
    return *this;
}

Tag::~Tag()
{
    if(_target != nullptr) {
        delete _target;
    }
}

QString Tag::shortName() const
{
    QString result;
    QStringList parts = _name.split('/');
    if(parts.count() > 0) {
        result = parts.last();
    }
    return result;
}

LightweightTag Tag::toLightweightTag() const
{
    LightweightTag tag;
    const LightweightTag* casted = dynamic_cast<const LightweightTag*>(this);
    if(casted != nullptr) {
        tag = *casted;
    }
    return tag;
}

AnnotatedTag Tag::toAnnotatedTag() const
{
    AnnotatedTag tag;
    const AnnotatedTag* casted = dynamic_cast<const AnnotatedTag*>(this);
    if(casted != nullptr) {
        tag = *casted;
    }
    return tag;
}

