#ifndef PROGRESSCALLBACK_H
#define PROGRESSCALLBACK_H
#include <Qt>

namespace GIT {

class ProgressCallback
{
public:
    virtual void progress(uint32_t receivedBytes, uint32_t receivedObjects, uint32_t totalObjects) = 0;
};

} // namespace GIT

#endif // PROGRESSCALLBACK_H
