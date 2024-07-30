#ifndef CHECKOUTOPTIONS_H
#define CHECKOUTOPTIONS_H
#include <git2.h>

namespace GIT {

class CheckoutOptions
{
public:
    CheckoutOptions();

    const git_checkout_options* options() const { return &_options; }

private:
    git_checkout_options _options;
};

} // namespace GIT

#endif // CHECKOUTOPTIONS_H
