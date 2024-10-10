/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a concrete implemetation of a lightweight tag.
 * As such, it is a trivial subclass of the abstract Tag class.
 *
 * Stephen Punak, August 1, 2024
*/

#ifndef LIGHTWEIGHTTAG_H
#define LIGHTWEIGHTTAG_H
#include <git2qt/tag.h>
#include <git2qt/gittypes.h>

namespace GIT {

class Repository;
class GIT2QT_EXPORT LightweightTag : public Tag
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
