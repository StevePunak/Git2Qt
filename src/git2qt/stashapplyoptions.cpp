#include "stashapplyoptions.h"

using namespace GIT;

const git_stash_apply_options* StashApplyOptions::toNative()
{
    _opts = GIT_STASH_APPLY_OPTIONS_INIT;
    _opts.flags = _applyModifiers;
    _opts.checkout_options = *_checkoutOptions.toNative();
    return &_opts;
}
