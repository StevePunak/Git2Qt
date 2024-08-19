/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Common types for libgit2qt.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GITTYPES_H
#define GITTYPES_H
#include <Qt>

namespace GIT {

enum FileStatus
{
    /// <summary>
    /// The file doesn't exist.
    /// </summary>
    Nonexistent = (1 << 31),

    /// <summary>
    /// The file hasn't been modified.
    /// </summary>
    Unaltered = 0, /* GIT_STATUS_CURRENT */

    /// <summary>
    /// New file has been added to the Index. It's unknown from the Head.
    /// </summary>
    NewInIndex = (1 << 0), /* GIT_STATUS_INDEX_NEW */

    /// <summary>
    /// New version of a file has been added to the Index. A previous version exists in the Head.
    /// </summary>
    ModifiedInIndex = (1 << 1), /* GIT_STATUS_INDEX_MODIFIED */

    /// <summary>
    /// The deletion of a file has been promoted from the working directory to the Index. A previous version exists in the Head.
    /// </summary>
    DeletedFromIndex = (1 << 2), /* GIT_STATUS_INDEX_DELETED */

    /// <summary>
    /// The renaming of a file has been promoted from the working directory to the Index. A previous version exists in the Head.
    /// </summary>
    RenamedInIndex = (1 << 3), /* GIT_STATUS_INDEX_RENAMED */

    /// <summary>
    /// A change in type for a file has been promoted from the working directory to the Index. A previous version exists in the Head.
    /// </summary>
    TypeChangeInIndex = (1 << 4), /* GIT_STATUS_INDEX_TYPECHANGE */

    /// <summary>
    /// New file in the working directory, unknown from the Index and the Head.
    /// </summary>
    NewInWorkdir = (1 << 7), /* GIT_STATUS_WT_NEW */

    /// <summary>
    /// The file has been updated in the working directory. A previous version exists in the Index.
    /// </summary>
    ModifiedInWorkdir = (1 << 8), /* GIT_STATUS_WT_MODIFIED */

    /// <summary>
    /// The file has been deleted from the working directory. A previous version exists in the Index.
    /// </summary>
    DeletedFromWorkdir = (1 << 9), /* GIT_STATUS_WT_DELETED */

    /// <summary>
    /// The file type has been changed in the working directory. A previous version exists in the Index.
    /// </summary>
    TypeChangeInWorkdir = (1 << 10), /* GIT_STATUS_WT_TYPECHANGE */

    /// <summary>
    /// The file has been renamed in the working directory.  The previous version at the previous name exists in the Index.
    /// </summary>
    RenamedInWorkdir = (1 << 11), /* GIT_STATUS_WT_RENAMED */

    /// <summary>
    /// The file is unreadable in the working directory.
    /// </summary>
    Unreadable = (1 << 12), /* GIT_STATUS_WT_UNREADABLE */

    /// <summary>
    /// The file is <see cref="NewInWorkdir"/> but its name and/or path matches an exclude pattern in a <c>gitignore</c> file.
    /// </summary>
    Ignored = (1 << 14), /* GIT_STATUS_IGNORED */

    /// <summary>
    /// The file is <see cref="Conflicted"/> due to a merge.
    /// </summary>
    Conflicted = (1 << 15), /* GIT_STATUS_CONFLICTED */
};
Q_DECLARE_FLAGS(FileStatuses, FileStatus)

enum Direction
{
    DirectionFetch = 0,
    DirectionPush  = 1,
};

enum GitStatusOptionFlags
{
    IncludeUntracked = (1 << 0),
    IncludeIgnored = (1 << 1),
    IncludeUnmodified = (1 << 2),
    ExcludeSubmodules = (1 << 3),
    RecurseUntrackedDirs = (1 << 4),
    DisablePathspecMatch = (1 << 5),
    RecurseIgnoredDirs = (1 << 6),
    RenamesHeadToIndex = (1 << 7),
    RenamesIndexToWorkDir = (1 << 8),
    SortCaseSensitively = (1 << 9),
    SortCaseInsensitively = (1 << 10),
    RenamesFromRewrites = (1 << 11),
    NoRefresh = (1 << 12),
    UpdateIndex = (1 << 13),
    IncludeUnreadable = (1 << 14),
    IncludeUnreadableAsUntracked = (1 << 15),
};

enum GitStatusShow
{
    IndexAndWorkDir = 0,
    IndexOnly = 1,
    WorkDirOnly = 2,
};

enum DiffAlgorithm
{
    /// <summary>
    /// The basic greedy diff algorithm.
    /// </summary>
    Myers = 0,

    /// <summary>
    /// Use "minimal diff" algorithm when generating patches.
    /// </summary>
    Minimal = 1,

    /// <summary>
    /// Use "patience diff" algorithm when generating patches.
    /// </summary>
    Patience = 2,
};

/// <summary>
/// Git specific modes for entries.
/// </summary>
enum Mode
{
    // Inspired from http://stackoverflow.com/a/8347325/335418

    /// <summary>
    /// 000000 file mode (the entry doesn't exist or is unreadable)
    /// </summary>
    NonexistentFile = 0,

    /// <summary>
    /// 040000 file mode
    /// </summary>
    Directory = 0x4000,

    /// <summary>
    /// 100644 file mode
    /// </summary>
    NonExecutableFile = 0x81A4,

    /// <summary>
    /// Obsolete 100664 file mode.
    /// <para>0100664 mode is an early Git design mistake. It's kept for
    ///   ascendant compatibility as some <see cref="Tree"/> and
    ///   <see cref="Repository.Index"/> entries may still bear
    ///   this mode in some old git repositories, but it's now deprecated.
    /// </para>
    /// </summary>
    NonExecutableGroupWritableFile = 0x81B4,

    /// <summary>
    /// 100755 file mode
    /// </summary>
    ExecutableFile = 0x81ED,

    /// <summary>
    /// 120000 file mode
    /// </summary>
    SymbolicLink = 0xA000,

    /// <summary>
    /// 160000 file mode
    /// </summary>
    GitLink = 0xE000
};

/// <summary>
/// The kind of changes that a Diff can report.
/// </summary>
enum ChangeKind {
    /// <summary>
    /// No changes detected.
    /// </summary>
    ChangeKindUnmodified = 0,

    /// <summary>
    /// The file was added.
    /// </summary>
    ChangeKindAdded = 1,

    /// <summary>
    /// The file was deleted.
    /// </summary>
    ChangeKindDeleted = 2,

    /// <summary>
    /// The file content was modified.
    /// </summary>
    ChangeKindModified = 3,

    /// <summary>
    /// The file was renamed.
    /// </summary>
    ChangeKindRenamed = 4,

    /// <summary>
    /// The file was copied.
    /// </summary>
    ChangeKindCopied = 5,

    /// <summary>
    /// The file is ignored in the workdir.
    /// </summary>
    ChangeKindIgnored = 6,

    /// <summary>
    /// The file is untracked in the workdir.
    /// </summary>
    ChangeKindUntracked = 7,

    /// <summary>
    /// The type (i.e. regular file, symlink, submodule, ...)
    /// of the file was changed.
    /// </summary>
    ChangeKindTypeChanged = 8,

    /// <summary>
    /// Entry is unreadable.
    /// </summary>
    ChangeKindUnreadable = 9,

    /// <summary>
    /// Entry is currently in conflict.
    /// </summary>
    ChangeKindConflicted = 10,
};

enum DiffModifier
{
    /// <summary>
    /// No special behavior.
    /// </summary>
    DiffModNone = 0,

    /// <summary>
    /// Include untracked files among the files to be processed, when
    /// diffing against the working directory.
    /// </summary>
    DiffModIncludeUntracked = (1 << 1),

    /// <summary>
    /// Include unmodified files among the files to be processed, when
    /// diffing against the working directory.
    /// </summary>
    DiffModIncludeUnmodified = (1 << 2),

    /// <summary>
    /// Treats the passed pathspecs as explicit paths (no pathspec match).
    /// </summary>
    DiffModDisablePathspecMatch = (1 << 3),

    /// <summary>
    /// Include ignored files among the files to be processed, when
    /// diffing against the working directory.
    /// </summary>
    DiffModIncludeIgnored = (1 << 4),
};
Q_DECLARE_FLAGS(DiffModifiers, DiffModifier)

class CurrentOperation
{
public:
    /// <summary>
    /// Determines the pending operation of a git repository - ie, whether
    /// an operation (merge, cherry-pick, etc) is in progress.
    /// </summary>
    enum Op
    {
        /// <summary>
        /// No operation is in progress.
        /// </summary>
        None = 0,

        /// <summary>
        /// A merge is in progress.
        /// </summary>
        Merge = 1,

        /// <summary>
        /// A revert is in progress.
        /// </summary>
        Revert = 2,

        /// <summary>
        /// A sequencer revert is in progress.
        /// </summary>
        RevertSequence = 3,

        /// <summary>
        /// A cherry-pick is in progress.
        /// </summary>
        CherryPick = 4,

        /// <summary>
        /// A sequencer cherry-pick is in progress.
        /// </summary>
        CherryPickSequence = 5,

        /// <summary>
        /// A bisect is in progress.
        /// </summary>
        Bisect = 6,

        /// <summary>
        /// A rebase is in progress.
        /// </summary>
        Rebase = 7,

        /// <summary>
        /// A rebase --interactive is in progress.
        /// </summary>
        RebaseInteractive = 8,

        /// <summary>
        /// A rebase --merge is in progress.
        /// </summary>
        RebaseMerge = 9,

        /// <summary>
        /// A mailbox application (am) is in progress.
        /// </summary>
        ApplyMailbox = 10,

        /// <summary>
        /// A mailbox application (am) or rebase is in progress.
        /// </summary>
        ApplyMailboxOrRebase = 11,
    };
};

/// <summary>
/// Disambiguates the different versions of an index entry during a merge.
/// </summary>
enum StageLevel
{
    /// <summary>
    /// The standard fully merged state for an index entry.
    /// </summary>
    Staged = 0,

    /// <summary>
    /// Version of the entry as it was in the common base merge commit.
    /// </summary>
    Ancestor = 1,

    /// <summary>
    /// Version of the entry as it is in the commit of the Head.
    /// </summary>
    Ours = 2,

    /// <summary>
    /// Version of the entry as it is in the commit being merged.
    /// </summary>
    Theirs = 3,
};

/// <summary>
/// Specifies the level of configuration to use.
/// </summary>
enum ConfigurationLevel
{
    /// <summary>
    /// The local .git/config of the current repository.
    /// </summary>
    Local = 5,

    /// <summary>
    /// The global ~/.gitconfig of the current user.
    /// </summary>
    Global = 4,

    /// <summary>
    /// The global ~/.config/git/config of the current user.
    /// </summary>
    Xdg = 3,

    /// <summary>
    /// The system wide .gitconfig.
    /// </summary>
    System = 2,

    /// <summary>
    /// Another system-wide configuration on Windows.
    /// </summary>
    ProgramData = 1,
};

/**
 * Flags for the delta object and the file objects on each side.
 *
 * These flags are used for both the `flags` value of the `git_diff_delta`
 * and the flags for the `git_diff_file` objects representing the old and
 * new sides of the delta.  Values outside of this public range should be
 * considered reserved for internal or future use.
 */
enum DiffDeltaFlag
{
    GIT_DIFF_FLAG_BINARY     = (1u << 0), /**< file(s) treated as binary data */
    GIT_DIFF_FLAG_NOT_BINARY = (1u << 1), /**< file(s) treated as text data */
    GIT_DIFF_FLAG_VALID_ID   = (1u << 2), /**< `id` value is known correct */
    GIT_DIFF_FLAG_EXISTS     = (1u << 3), /**< file exists at this side of the delta */
    GIT_DIFF_FLAG_VALID_SIZE = (1u << 4)  /**< file size value is known correct */
};
Q_DECLARE_FLAGS(DiffDeltaFlags, DiffDeltaFlag)

enum DeltaType
{
    DeltaUnmodified = 0,    /**< no changes */
    DeltaAdded = 1,         /**< entry does not exist in old version */
    DeltaDeleted = 2,       /**< entry does not exist in new version */
    DeltaModified = 3,      /**< entry content changed between old and new */
    DeltaRenamed = 4,       /**< entry was renamed between old and new */
    DeltaCopied = 5,        /**< entry was copied from another old entry */
    DeltaIgnored = 6,       /**< entry is ignored item in workdir */
    DeltaUntracked = 7,     /**< entry is untracked item in workdir */
    DeltaTypeCange = 8,     /**< type of entry changed between old and new */
    DeltaUnreadable = 9,    /**< entry is unreadable */
    DeltaConflicted = 10    /**< entry in the index is conflicted */
};

enum ReferenceType
{
    UnknownReferenceType,
    SymbolicReferenceType,
    DirectReferenceType,
};

enum ObjectType
{
    ObjectTypeAny =         -2, /**< Object can be any of the following */
    ObjectTypeInvalid =     -1, /**< Object is invalid. */
    ObjectTypeCommit =      1, /**< A commit object. */
    ObjectTypeTree =        2, /**< A tree (directory listing) object. */
    ObjectTypeBlob =        3, /**< A file revision object. */
    ObjectTypeTag =         4, /**< An annotated tag object. */
    ObjectTypeDelta =       6, /**< A delta, base is given by an offset. */
    ObjectTypeRefDelta =    7  /**< A delta, base is given by object id. */
};

enum TreeEntryTargetType
{
    /// <summary>
    /// A file revision object.
    /// </summary>
    TreeEntryTargetBlob,

    /// <summary>
    /// A tree object.
    /// </summary>
    TreeEntryTargetTree,

    /// <summary>
    /// A pointer to a commit object in another repository.
    /// </summary>
    TreeEntryTargetGitLink,
};

enum BranchType
{
    UnknownBranchType = 0,
    LocalBranch,
    RemoteBranch
};

enum DiffTargets
{
    /// <summary>
    /// The repository index.
    /// </summary>
    DiffTargetIndex = 1,

    /// <summary>
    /// The working directory.
    /// </summary>
    DiffTargetWorkingDirectory = 2,
};

enum GitDiffFindFlag
{
    // Obey `diff.renames`. Overridden by any other GIT_DIFF_FIND_... flag.
    DiffFindByConfig = 0,

    // Look for renames? (`--find-renames`)
    DiffFindRenames = (1 << 0),
    // consider old side of modified for renames? (`--break-rewrites=N`)
    DiffFindRenamesFromRewrites = (1 << 1),

    // look for copies? (a la `--find-copies`)
    DiffFindCopies = (1 << 2),
    // consider unmodified as copy sources? (`--find-copies-harder`)
    DiffFindCopiesFromUnmodified = (1 << 3),

    // mark large rewrites for split (`--break-rewrites=/M`)
    DiffFindRewrites = (1 << 4),
    // actually split large rewrites into delete/add pairs
    DiffFindBreakRewrites = (1 << 5),
    // mark rewrites for split and break into delete/add pairs
    DiffFindAndBreakRewrites =
    (DiffFindRewrites | DiffFindBreakRewrites),

    // find renames/copies for untracked items in working directory
    DiffFindForUntracked = (1 << 6),

    // turn on all finding features
    DiffFindAll = (0x0ff),

    // measure similarity ignoring leading whitespace (default)
    DiffFindIgnoreLeadingWhitespace = 0,
    // measure similarity ignoring all whitespace
    DiffFindIgnoreWhitespace = (1 << 12),
    // measure similarity including all data
    DiffFindDontIgnoreWhitespace = (1 << 13),
    // measure similarity only by comparing SHAs (fast and cheap)
    DiffFindExactMatchOnly = (1 << 14),

    // do not break rewrites unless they contribute to a rename
    DiffBreakRewritesForRenamesOnly = (1 << 15),

    // Remove any UNMODIFIED deltas after find_similar is done.
    DiffFindRemoveUnmodified = (1 << 16),
};
Q_DECLARE_FLAGS(GitDiffFindFlags, GitDiffFindFlag)


QString getFileStatusString(FileStatuses value);
FileStatus getFileStatus(const QString& value);
QList<FileStatus> getFileStatusValues();

QString getConfigurationLevelString(ConfigurationLevel value);
ConfigurationLevel getConfigurationLevel(const QString& value);
QList<ConfigurationLevel> getConfigurationLevelValues();

QString getDeltaTypeString(DeltaType value);
DeltaType getDeltaType(const QString& value);
QList<DeltaType> getDeltaTypeValues();

QString getDiffDeltaFlagString(DiffDeltaFlag value);
DiffDeltaFlag getDiffDeltaFlag(const QString& value);
QList<DiffDeltaFlag> getDiffDeltaFlagValues();

QString getReferenceTypeString(ReferenceType value);
ReferenceType getReferenceType(const QString& value);
QList<ReferenceType> getReferenceTypeValues();

QString getObjectTypeString(ObjectType value);
ObjectType getObjectType(const QString& value);
QList<ObjectType> getObjectTypeValues();

QString getModeString(Mode value);
Mode getMode(const QString& value);
QList<Mode> getModeValues();

QString getStageLevelString(StageLevel value);
StageLevel getStageLevel(const QString& value);
QList<StageLevel> getStageLevelValues();

QString getTreeEntryTargetTypeString(TreeEntryTargetType value);
TreeEntryTargetType getTreeEntryTargetType(const QString& value);
QList<TreeEntryTargetType> getTreeEntryTargetTypeValues();

QString getBranchTypeString(BranchType value);
BranchType getBranchType(const QString& value);
QList<BranchType> getBranchTypeValues();

} // namespace GIT

Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffDeltaFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::FileStatuses)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::GitDiffFindFlags)

#endif // GITTYPES_H
