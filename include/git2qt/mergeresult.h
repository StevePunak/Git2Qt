#ifndef MERGERESULT_H
#define MERGERESULT_H
#include <git2qt/commit.h>

namespace GIT {

class MergeResult
{
public:
    enum MergeStatus
    {
        /// <summary>
        /// Merge was up-to-date.
        /// </summary>
        UpToDate,

        /// <summary>
        /// Fast-forward merge.
        /// </summary>
        FastForward,

        /// <summary>
        /// Non-fast-forward merge.
        /// </summary>
        NonFastForward,

        /// <summary>
        /// Merge resulted in conflicts.
        /// </summary>
        Conflicts,

        Invalid,
    };

    MergeResult() {}
    MergeResult(MergeStatus status, const Commit& commit = Commit()) :
        _status(status), _commit(commit) {}

    MergeStatus status() const { return _status; }
    Commit commit() const { return _commit; }

    bool isValid() const { return _status != Invalid; }

private:
    MergeStatus _status = Invalid;
    Commit _commit;
};

} // namespace GIT

#endif // MERGERESULT_H
