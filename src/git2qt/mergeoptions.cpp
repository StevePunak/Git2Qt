#include "mergeoptions.h"

using namespace GIT;

const git_merge_options* MergeOptions::toNative()
{
    _opts = GIT_MERGE_OPTIONS_INIT;
    _opts.flags = _flags;
    _opts.rename_threshold = _renameThreshold;
    _opts.target_limit = _targetLimit;
    _opts.file_favor = (git_merge_file_favor_t)_favor;
    _opts.file_flags = (git_merge_file_flag_t)_fileFlags;

    return &_opts;
}
