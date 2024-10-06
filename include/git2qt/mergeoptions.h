#ifndef MERGEOPTIONS_H
#define MERGEOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2.h>

namespace GIT {

class MergeOptions
{
public:
    MergeOptions() {}

    const git_merge_options* toNative();

private:
    git_merge_options _opts;
};

} // namespace GIT

#endif // MERGEOPTIONS_H
