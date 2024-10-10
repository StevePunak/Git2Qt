#include "fetchoptions.h"

#include <stringarray.h>

using namespace GIT;

const git_fetch_options* FetchOptions::toNative()
{
    makeNative(&_opts);
    return &_opts;
}

void FetchOptions::makeNative(git_fetch_options* options)
{
    *options = GIT_FETCH_OPTIONS_INIT;
    options->download_tags = (git_remote_autotag_option_t)_tagFetchMode;
    options->prune = _prune ? GIT_FETCH_PRUNE : GIT_FETCH_NO_PRUNE;
    _customHeadersNative = StringArray(_customHeaders);
    options->custom_headers = *_customHeadersNative.toNative();
}
