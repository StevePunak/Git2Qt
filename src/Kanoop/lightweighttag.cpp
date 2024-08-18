#include "lightweighttag.h"

#include <blob.h>
#include <gitexception.h>
#include <repository.h>
#include <tree.h>

using namespace GIT;

LightweightTag::LightweightTag() :
    Tag(TagLightweightEntity, nullptr, QString(), ObjectId())
{
}

LightweightTag::LightweightTag(Repository* repo, const QString& name, const ObjectId& objectId) :
    Tag(TagLightweightEntity, repo, name, objectId)
{
    commonInit();
}

LightweightTag::LightweightTag(const LightweightTag& other) :
    Tag(TagLightweightEntity, other.repository(), other.name(), other.objectId())
{
    *this = other;
}

LightweightTag& LightweightTag::operator=(const LightweightTag& other)
{
    Tag::operator =(other);
    commonInit();
    return *this;
}

LightweightTag::~LightweightTag()
{
}

void LightweightTag::commonInit()
{
    try
    {
        ObjectType objectType = GitObject::getObjectType(repository(), objectId());
        switch (objectType) {
        case ObjectTypeCommit:
            setTarget(new Commit(repository(), objectId()));
            break;
        case ObjectTypeBlob:
            setTarget(new Blob(repository(), objectId()));
            break;
        case ObjectTypeTree:
            setTarget(new Tree(repository(), objectId()));
            break;
        default:
            break;
        }
    }
    catch(const GitException&)
    {
    }
}
