#ifndef STASHAPPLYOPTIONS_H
#define STASHAPPLYOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2qt/checkoutoptions.h>

namespace GIT {

class StashApplyOptions
{
public:
    StashApplyOptions() {}

    StashApplyModifiers applyModifiers() const { return _applyModifiers; }
    void setApplyModifiers(StashApplyModifiers value) { _applyModifiers = value; }

    CheckoutOptions checkoutOptions() const { return _checkoutOptions; }
    void setCheckoutOptions(const CheckoutOptions& value) { _checkoutOptions = value; }

    const git_stash_apply_options* toNative();

private:
    StashApplyModifiers _applyModifiers = StashApplyDefault;
    CheckoutOptions _checkoutOptions;

    git_stash_apply_options _opts;
};

} // namespace GIT

#endif // STASHAPPLYOPTIONS_H
