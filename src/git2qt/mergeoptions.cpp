#include "mergeoptions.h"

using namespace GIT;

const git_merge_options* MergeOptions::toNative()
{
    _opts = GIT_MERGE_OPTIONS_INIT;
    return &_opts;
}
