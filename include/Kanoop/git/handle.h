#ifndef HANDLE_H
#define HANDLE_H
#include <git2.h>

namespace GIT {

template <typename T>
class Handle
{
public:
    Handle(T handle) :
        _handle(handle) {}
    virtual ~Handle() {}

    T value() const { return _handle; }

protected:
    T _handle;
};

class ReferenceHandle : public Handle<git_reference*>
{
public:
    ReferenceHandle(git_reference* handle) :
        Handle(handle) {}
    virtual ~ReferenceHandle()
    {
        git_reference_free(_handle);
    }
};

} // namespace GIT

#endif // HANDLE_H
