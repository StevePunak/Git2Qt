#ifndef FETCHOPTIONS_H
#define FETCHOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2qt/stringarray.h>
#include <git2.h>

namespace GIT {

class FetchOptions
{
public:
    FetchOptions() {}

    TagFetchMode tagFetchMode() const { return _tagFetchMode; }
    void setTagFetchMode(TagFetchMode value) { _tagFetchMode = value; }

    bool prune() const { return _prune; }
    void setPrune(bool value) { _prune = value; }

    QStringList customHeaders() const { return _customHeaders; }
    void setCustomHeaders(QStringList value) { _customHeaders = value; }

    const git_fetch_options* toNative();
    void makeNative(git_fetch_options* options);

private:
    TagFetchMode _tagFetchMode = RemoteDownloadTagsUnspecified;
    bool _prune = false;
    QStringList _customHeaders;

    StringArray _customHeadersNative;

    git_fetch_options _opts;
};

} // namespace GIT

#endif // FETCHOPTIONS_H
