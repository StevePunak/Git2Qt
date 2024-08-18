#include "annotatedtag.h"

#include <gitexception.h>
#include <repository.h>

#include "log.h"

using namespace GIT;

AnnotatedTag::AnnotatedTag() :
    Tag(TagAnnotatedEntity, nullptr, QString(), ObjectId())
{
}

AnnotatedTag::AnnotatedTag(Repository* repo, const QString& name, const ObjectId& objectId) :
    Tag(TagAnnotatedEntity, repo, name, objectId)
{
    commonInit();
}

AnnotatedTag::AnnotatedTag(const AnnotatedTag& other) :
    Tag(TagLightweightEntity, other.repository(), other.name(), other.objectId())
{
    *this = other;
}

AnnotatedTag& AnnotatedTag::operator=(const AnnotatedTag& other)
{
    Tag::operator =(other);
    _message = other._message;
    _signature = other._signature;
    commonInit();
    return *this;
}

AnnotatedTag::~AnnotatedTag()
{
}

void AnnotatedTag::commonInit()
{
    TagHandle handle = createHandle();
    try
    {
        throwIfTrue(handle.isNull());
        _message = git_tag_message(handle.value());
        setName(git_tag_name(handle.value()));
        _signature = Signature(git_tag_tagger(handle.value()));
        ObjectType targetType = (ObjectType)git_tag_target_type(handle.value());
        switch(targetType) {
        case ObjectTypeCommit:
        {
            const git_oid* targetOid = git_tag_target_id(handle.value());
            throwIfNull(targetOid);
            setTarget(new Commit(repository(), targetOid));
            break;
        }
        default:
            throw GitException(QString("Unhandled target type %1").arg(getObjectTypeString(targetType)));
        }

    }
    catch(const GitException& e)
    {
        Log::sysLogText(KLOG_ERROR, QString("Annotated tag create failure: %1").arg(e.message()));
    }
}

TagHandle AnnotatedTag::createHandle() const
{
    TagHandle handle;
    git_tag* tag = nullptr;
    if(git_tag_lookup(&tag, repository()->handle().value(), objectId().toNative()) == 0) {
        handle = TagHandle(tag);
    }
    return handle;
}
