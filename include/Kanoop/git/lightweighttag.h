#ifndef LIGHTWEIGHTTAG_H
#define LIGHTWEIGHTTAG_H
#include <Kanoop/git/tag.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class Repository;
class LightweightTag : public Tag
{
public:
    LightweightTag();
    LightweightTag(Repository* repo, const QString& name, const ObjectId& objectId);
    LightweightTag(const LightweightTag& other);
    LightweightTag& operator=(const LightweightTag& other);
    virtual ~LightweightTag();

private:
    void commonInit();
};

} // namespace GIT

#endif // LIGHTWEIGHTTAG_H
