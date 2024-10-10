#ifndef MERGEOPTIONS_H
#define MERGEOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2.h>

namespace GIT {

class MergeOptions
{
public:
    MergeOptions() {}

    enum MergeFileFavor
    {
        /// <summary>
        /// When a region of a file is changed in both branches, a conflict
        /// will be recorded in the index so that the checkout operation can produce
        /// a merge file with conflict markers in the working directory.
        /// This is the default.
        /// </summary>
        Normal = 0,

        /// <summary>
        /// When a region of a file is changed in both branches, the file
        /// created in the index will contain the "ours" side of any conflicting
        /// region. The index will not record a conflict.
        /// </summary>
        Ours = 1,

        /// <summary>
        /// When a region of a file is changed in both branches, the file
        /// created in the index will contain the "theirs" side of any conflicting
        /// region. The index will not record a conflict.
        /// </summary>
        Theirs = 2,

        /// <summary>
        /// When a region of a file is changed in both branches, the file
        /// created in the index will contain each unique line from each side,
        /// which has the result of combining both files. The index will not
        /// record a conflict.
        /// </summary>
        Union = 3,
    };

    MergeFlags flags() const { return _flags; }
    void setFlags(MergeFlags value) { _flags = value; }

    FastForwardStrategies fastForwardStrategy() const { return _fastForwardStrategy; }
    void setFastForwardStrategy(FastForwardStrategies value) { _fastForwardStrategy = value; }

    bool commitOnSuccess() const { return _commitOnSuccess; }
    void setCommitOnSuccess(bool value) { _commitOnSuccess = value; }

    CheckoutNotifyFlag checkoutNotifyFlags() const { return _checkoutNotifyFlags; }
    void setCheckoutNotifyFlags(CheckoutNotifyFlag value) { _checkoutNotifyFlags = value; }

    CheckoutFileConflictStrategy checkoutFileConflictStrategy() const { return _checkoutFileConflictStrategy; }
    void setCheckoutFileConflictStrategy(CheckoutFileConflictStrategy value) { _checkoutFileConflictStrategy = value; }

    CheckoutStrategy checkoutStrategy() const { return _checkoutStrategy; }
    void setCheckoutStrategy(CheckoutStrategy value) { _checkoutStrategy = value; }

    MergeFileFlag fileFlags() const { return _fileFlags; }
    void setFileFlags(MergeFileFlag value) { _fileFlags = value; }

    bool findRenames() const { return _findRenames; }
    void setFindRenames(bool value) { _findRenames = value; }

    bool failOnConflict() const { return _failOnConflict; }
    void setFailOnConflict(bool value) { _failOnConflict = value; }

    bool skipReuc() const { return _skipReuc; }
    void setSkipReuc(bool value) { _skipReuc = value; }

    int renameThreshold() const { return _renameThreshold; }
    void setRenameThreshold(int value) { _renameThreshold = value; }

    int targetLimit() const { return _targetLimit; }
    void setTargetLimit(int value) { _targetLimit = value; }

    MergeFileFavor favor() const { return _favor; }
    void setFavor(MergeFileFavor value) { _favor = value; }

    bool ignoreWhitespaceChange() const { return _ignoreWhitespaceChange; }
    void setIgnoreWhitespaceChange(bool value) { _ignoreWhitespaceChange = value; }

    const git_merge_options* toNative();

private:
    MergeFlags _flags = MergeFlagNormal;
    FastForwardStrategies _fastForwardStrategy = FastForwardDefault;
    CheckoutStrategy _checkoutStrategy = CheckoutStrategySafe;
    CheckoutNotifyFlag _checkoutNotifyFlags = CheckoutNotifyNone;
    CheckoutFileConflictStrategy _checkoutFileConflictStrategy = CheckoutFileConflictNormal;
    MergeFileFlag _fileFlags = MergeFileDefault;
    bool _commitOnSuccess = true;
    bool _findRenames = true;
    bool _failOnConflict =  false;
    bool _skipReuc = false;
    int _renameThreshold = 50;
    int _targetLimit = 200;
    MergeFileFavor _favor = Normal;
    bool _ignoreWhitespaceChange;

    git_merge_options _opts;
};

} // namespace GIT

#endif // MERGEOPTIONS_H
