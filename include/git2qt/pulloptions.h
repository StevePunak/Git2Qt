#ifndef PULLOPTIONS_H
#define PULLOPTIONS_H
#include <git2qt/mergeoptions.h>
#include <git2qt/fetchoptions.h>

namespace GIT {

class PullOptions
{
public:
    PullOptions() {}

    FetchOptions fetchOptions() const { return _fetchOptions; }
    void setFetchOptions(FetchOptions value) { _fetchOptions = value; }

    MergeOptions mergeOptions() const { return _mergeOptions; }
    void setMergeOptions(MergeOptions value) { _mergeOptions = value; }

private:
    FetchOptions _fetchOptions;
    MergeOptions _mergeOptions;
};

} // namespace GIT

#endif // PULLOPTIONS_H
