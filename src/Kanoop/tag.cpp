#include "tag.h"
#include "repository.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

#include <blob.h>
#include <tree.h>

using namespace GIT;

Tag::Tag() :
    GitObject(TagEntity, nullptr, ObjectId()) {}

Tag::Tag(Repository* repo, const QString& name, const ObjectId& objectId) :
    GitObject(TagEntity, repo, objectId),
    _name(name)
{
    commonInit();
}

Tag::Tag(const Tag& other) :
    GitObject(TagEntity, repository(), other.objectId())
{
    *this = other;
}

Tag& Tag::operator=(const Tag& other)
{
    GitObject::operator =(other);
    _name = other._name;
    commonInit();
    return *this;
}

Tag::~Tag()
{
    if(_target != nullptr) {
        delete _target;
    }
}

void Tag::commonInit()
{
    try
    {
        ObjectType objectType = GitObject::getObjectType(repository(), objectId());
        switch (objectType) {
        case ObjectTypeCommit:
            _target = new Commit(repository(), objectId());
            break;
        case ObjectTypeTag:
            _target = new Tag(repository(), QString(), objectId());
            KLog::sysLogText(KLOG_WARNING, "Creating nameless tag... I think this illegal");
            break;
        case ObjectTypeBlob:
            _target = new Blob(repository(), objectId());
            break;
        case ObjectTypeTree:
            _target = new Tree(repository(), objectId());
            break;
        default:
            break;
        }
    }
    catch(const CommonException&)
    {
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

