#ifndef TAGCOLLECTION_H
#define TAGCOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/tag.h>
#include <Kanoop/git/tagannotation.h>

namespace GIT {

class Commit;
class Repository;
class Signature;
class TagCollection : public GitEntity
{
public:
    TagCollection(Repository* repo);

    Tag createLightweightTag(const QString& name, const GitObject& targetObject);
    Tag createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject);
    Tag findTag(const QString& name) const;
    bool deleteLocalTag(const QString& name);

    Tag::List tags() const { return _lightweightTags; }

    virtual bool isNull() const override;

private:
    Tag::List retrieveTags();

    // Callbacks
    static int gitTagForeachCallback(const char *name, git_oid *oid, void *payload);

    Tag::List _lightweightTags;
    TagAnnotation::List _annotatedTags;
};

} // namespace GIT

#endif // TAGCOLLECTION_H
