#include "checkoutoptions.h"

using namespace GIT;

CheckoutOptions::CheckoutOptions()
{
    _options = GIT_CHECKOUT_OPTIONS_INIT;
    _options.checkout_strategy = GIT_CHECKOUT_SAFE;
}
