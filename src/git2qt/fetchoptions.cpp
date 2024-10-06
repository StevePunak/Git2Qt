#include "fetchoptions.h"

#include <stringarray.h>

using namespace GIT;

const git_fetch_options* FetchOptions::toNative()
{
    _opts = GIT_FETCH_OPTIONS_INIT;
    _opts.download_tags = (git_remote_autotag_option_t)_tagFetchMode;
    _opts.prune = _prune ? GIT_FETCH_PRUNE : GIT_FETCH_NO_PRUNE;
    _customHeadersNative = StringArray(_customHeaders);
    _opts.custom_headers = *_customHeadersNative.toNative();
    return &_opts;
}
