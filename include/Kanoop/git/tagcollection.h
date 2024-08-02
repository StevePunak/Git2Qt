#ifndef TAGCOLLECTION_H
#define TAGCOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/tag.h>

namespace GIT {
class Repository;

class TagCollection : public GitEntity
{
public:
    TagCollection(Repository* repo);

    Tag::List retrieveTags();

    virtual bool isNull() const override;

private:

    // Callbacks
    static int gitTagForeachCallback(const char *name, git_oid *oid, void *payload);

    Tag::List _retrieveTags;

};

} // namespace GIT

#endif // TAGCOLLECTION_H
