#include "checkoutoptions.h"

using namespace GIT;

const git_checkout_options* CheckoutOptions::toNative()
{
    _options = GIT_CHECKOUT_OPTIONS_INIT;
    _options.checkout_strategy = _modifiers == Force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
    if(_paths.count() > 0) {
        _pathsArray = StringArray(_paths);
        _options.paths = *_pathsArray.toNative();
    }

    return &_options;
}
