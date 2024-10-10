#include "checkoutoptions.h"

#include <mergeoptions.h>

using namespace GIT;

CheckoutOptions CheckoutOptions::fromMergeOptions(const MergeOptions& mergeOptions)
{
    CheckoutOptions result;

    if(mergeOptions.checkoutStrategy() & CheckoutStrategyForce) {
        result._modifiers = Force;
    }
    result._notifyFlags = mergeOptions.checkoutNotifyFlags();
    return result;
}

const git_checkout_options* CheckoutOptions::toNative()
{
    memset(&_options, 0, sizeof(_options));
    _options = GIT_CHECKOUT_OPTIONS_INIT;
    _options.checkout_strategy = _modifiers == Force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
    _options.notify_flags = _notifyFlags;
    if(_paths.count() > 0) {
        _pathsArray = StringArray(_paths);
        _options.paths = *_pathsArray.toNative();
    }

    return &_options;
}
