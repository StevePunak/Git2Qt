#ifndef ANNOTATEDTAG_H
#define ANNOTATEDTAG_H
#include <Kanoop/git/tag.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/signature.h>

namespace GIT {

class Repository;
class AnnotatedTag : public Tag
{
public:
    AnnotatedTag(Repository* repo, const QString& name, const ObjectId& objectId);
};

} // namespace GIT

#endif // ANNOTATEDTAG_H
