#ifndef PROGRESSCALLBACK_H
#define PROGRESSCALLBACK_H
#include <Qt>
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT ProgressCallback
{
public:
    virtual void progressCallback(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects) = 0;
};

} // namespace GIT

#endif // PROGRESSCALLBACK_H
