#ifndef TAGANNOTATION_H
#define TAGANNOTATION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class Repository;

class TagAnnotation : public GitEntity
{
public:
    TagAnnotation(Repository* repo);

    virtual bool isNull() const override { return false; }
};

} // namespace GIT

#endif // TAGANNOTATION_H
