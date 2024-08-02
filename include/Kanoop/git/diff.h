#ifndef DIFF_H
#define DIFF_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/diffdelta.h>

namespace GIT {
class TreeChanges;
class CompareOptions;
class StageOptions;
class Repository;
class Diff : public GitEntity
{
public:
    enum DiffModifier
    {
        /// <summary>
        /// No special behavior.
        /// </summary>
        None = 0,

        /// <summary>
        /// Include untracked files among the files to be processed, when
        /// diffing against the working directory.
        /// </summary>
        IncludeUntracked = (1 << 1),

        /// <summary>
        /// Include unmodified files among the files to be processed, when
        /// diffing against the working directory.
        /// </summary>
        IncludeUnmodified = (1 << 2),

        /// <summary>
        /// Treats the passed pathspecs as explicit paths (no pathspec match).
        /// </summary>
        DisablePathspecMatch = (1 << 3),

        /// <summary>
        /// Include ignored files among the files to be processed, when
        /// diffing against the working directory.
        /// </summary>
        IncludeIgnored = (1 << 4),
    };
    Q_DECLARE_FLAGS(DiffModifiers, DiffModifier)

    Diff(Repository* repo) :
        GitEntity(DiffEntity, repo) {}
    virtual ~Diff() {}

    TreeChanges compare(GIT::Diff::DiffModifiers diffModifiers, const QStringList& paths, const CompareOptions& compareOptions);
    GIT::DiffDelta::List listDiffs(const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::None);

    virtual bool isNull() const override { return false; }

private:
    void buildDiffOptions(DiffModifiers diffOptions, const CompareOptions& compareOptions, git_diff_options& options);

    // Callbacks
    static int fileCallback(const git_diff_delta *delta, float progress, void *payload);
    static int binaryCallback(const git_diff_delta *d, const git_diff_binary *binary, void *payload);
    static int hunkCallback(const git_diff_delta *d, const git_diff_hunk *h, void *payload);
    static int lineCallback(const git_diff_delta *d, const git_diff_hunk *h, const git_diff_line *l, void *payload);
};

} // namespace GIT

Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::Diff::DiffModifiers)

#endif // DIFF_H
