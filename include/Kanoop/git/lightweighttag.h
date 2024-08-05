#ifndef LIGHTWEIGHTTAG_H
#define LIGHTWEIGHTTAG_H
#include <Kanoop/git/tag.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class Repository;
class LightweightTag : public Tag
{
public:
    LightweightTag(Repository* repo, const QString& name, const ObjectId& objectId);
};

} // namespace GIT

#endif // LIGHTWEIGHTTAG_H
