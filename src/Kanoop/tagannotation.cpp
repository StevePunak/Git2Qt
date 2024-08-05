#include "tagannotation.h"
#include "repository.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

using namespace GIT;

TagAnnotation::TagAnnotation() :
    GitObject(TagAnnotationEntity, nullptr, ObjectId())
{
}

TagAnnotation::TagAnnotation(Repository* repo, const ObjectId& objectId) :
    GitObject(TagAnnotationEntity, repo, objectId)
{
    commonInit();
}

TagAnnotation::TagAnnotation(const TagAnnotation& other) :
    GitObject(TagAnnotationEntity, repository(), other.objectId())
{
    *this = other;
}

TagAnnotation& TagAnnotation::operator=(const TagAnnotation& other)
{
    GitObject::operator =(other);
    _name = other._name;
    _message = other._message;
    _signature = other._signature;
    commonInit();
    return *this;
}

void TagAnnotation::commonInit()
{
    TagHandle handle = createHandle();
    try
    {
        throwIfTrue(handle.isNull());
        _message = git_tag_message(handle.value());
        _name = git_tag_name(handle.value());
        _signature = Signature(git_tag_tagger(handle.value()));
        ObjectType targetType = (ObjectType)git_tag_target_type(handle.value());
        switch(targetType) {
        case ObjectTypeCommit:
        {
            const git_oid* targetOid = git_tag_target_id(handle.value());
            throwIfNull(targetOid);
            _target = new Commit(repository(), targetOid);
            break;
        }
        default:
            throw CommonException(QString("Unhandled target type %1").arg(getObjectTypeString(targetType)));
        }

    }
    catch(const CommonException& e)
    {
        KLog::sysLogText(KLOG_ERROR, QString("Annotated tag create failure: %1").arg(e.message()));
    }
}

TagAnnotation::~TagAnnotation()
{
    if(_target != nullptr) {
        delete _target;
    }
}

QString TagAnnotation::shortName() const
{
    QString result;
    QStringList parts = _name.split('/');
    if(parts.count() > 0) {
        result = parts.last();
    }
    return result;
}

TagHandle TagAnnotation::createHandle() const
{
    TagHandle handle;
    git_tag* tag = nullptr;
    if(git_tag_lookup(&tag, repository()->handle().value(), objectId().toNative()) == 0) {
        handle = TagHandle(tag);
    }
    return handle;
}
