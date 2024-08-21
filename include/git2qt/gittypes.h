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

enum SubmoduleIgnore
{
    /// <summary>
    /// Reset to the last saved ignore rule.
    /// </summary>
    Reset = -1,

    /// <summary>
    /// Any change or untracked == dirty
    /// </summary>
    None = 1,

    /// <summary>
    /// Dirty if tracked files change
    /// </summary>
    Untracked = 2,

    /// <summary>
    /// Only dirty if HEAD moved
    /// </summary>
    Dirty = 3,

    /// <summary>
    /// Never dirty
    /// </summary>
    All = 4,
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

enum DiffOptionFlag
{
    /// <summary>
    /// Normal diff, the default
    /// </summary>
    DiffOptionNormal = 0,

    /*
     * Options controlling which files will be in the diff
     */

    /// <summary>
    /// Reverse the sides of the diff
    /// </summary>
    DiffOptionReverse = (1 << 0),

    /// <summary>
    /// Include ignored files in the diff
    /// </summary>
    DiffOptionIncludeIgnored = (1 << 1),

    /// <summary>
    /// Even with GIT_DIFF_INCLUDE_IGNORED, an entire ignored directory
    /// will be marked with only a single entry in the diff; this flag
    /// adds all files under the directory as IGNORED entries, too.
    /// </summary>
    DiffOptionRecurseIgnoredDirs = (1 << 2),

    /// <summary>
    /// Include untracked files in the diff
    /// </summary>
    DiffOptionIncludeUntracked = (1 << 3),

    /// <summary>
    /// Even with GIT_DIFF_INCLUDE_UNTRACKED, an entire untracked
    /// directory will be marked with only a single entry in the diff
    /// (a la what core Git does in `git status`); this flag adds *all*
    /// files under untracked directories as UNTRACKED entries, too.
    /// </summary>
    DiffOptionRecurseUntrackedDirs = (1 << 4),

    /// <summary>
    /// Include unmodified files in the diff
    /// </summary>
    DiffOptionIncludeUnmodified = (1 << 5),

    /// <summary>
    /// Normally, a type change between files will be converted into a
    /// DELETED record for the old and an ADDED record for the new; this
    /// options enabled the generation of TYPECHANGE delta records.
    /// </summary>
    DiffOptionIncludeTypeChange = (1 << 6),

    /// <summary>
    /// Even with GIT_DIFF_INCLUDE_TYPECHANGE, blob->tree changes still
    /// generally show as a DELETED blob.  This flag tries to correctly
    /// label blob->tree transitions as TYPECHANGE records with new_file's
    /// mode set to tree.  Note: the tree SHA will not be available.
    /// </summary>
    DiffOptionIncludeTypechangeTrees = (1 << 7),

    /// <summary>
    /// Ignore file mode changes
    /// </summary>
    DiffOptionIgnoreFilemode = (1 << 8),

    /// <summary>
    /// Treat all submodules as unmodified
    /// </summary>
    DiffOptionIgnoreSubmodules = (1 << 9),

    /// <summary>
    /// Use case insensitive filename comparisons
    /// </summary>
    DiffOptionIgnoreCase = (1 << 10),


    /// <summary>
    /// May be combined with `GIT_DIFF_IGNORE_CASE` to specify that a file
    /// that has changed case will be returned as an add/delete pair.
    /// </summary>
    DiffOptionIncludeCasechange = (1 << 11),

    /// <summary>
    /// If the pathspec is set in the diff options, this flags means to
    /// apply it as an exact match instead of as an fnmatch pattern.
    /// </summary>
    DiffOptionDisablePathspecMatch = (1 << 12),

    /// <summary>
    /// Disable updating of the `binary` flag in delta records.  This is
    /// useful when iterating over a diff if you don't need hunk and data
    /// callbacks and want to avoid having to load file completely.
    /// </summary>
    DiffOptionSkipBinaryCheck = (1 << 13),

    /// <summary>
    /// When diff finds an untracked directory, to match the behavior of
    /// core Git, it scans the contents for IGNORED and UNTRACKED files.
    /// If *all* contents are IGNORED, then the directory is IGNORED; if
    /// any contents are not IGNORED, then the directory is UNTRACKED.
    /// This is extra work that may not matter in many cases.  This flag
    /// turns off that scan and immediately labels an untracked directory
    /// as UNTRACKED (changing the behavior to not match core Git).
    /// </summary>
    DiffOptionEnableFastUntrackedDirs = (1 << 14),

    /// <summary>
    /// When diff finds a file in the working directory with stat
    /// information different from the index, but the OID ends up being the
    /// same, write the correct stat information into the index.  Note:
    /// without this flag, diff will always leave the index untouched.
    /// </summary>
    DiffOptionUpdateIndex = (1 << 15),

    /// <summary>
    /// Include unreadable files in the diff
    /// </summary>
    DiffOptionIncludeUnreadable = (1 << 16),

    /// <summary>
    /// Include unreadable files in the diff
    /// </summary>
    DiffOptionIncludeUnreadableAsUntracked = (1 << 17),

    /*
     * Options controlling how output will be generated
     */

    /// <summary>
    /// Use a heuristic that takes indentation and whitespace into account
    /// which generally can produce better diffs when dealing with ambiguous
    /// diff hunks.
    /// </summary>
    DiffOptionIndentHeuristic = (1 << 18),

    /// <summary>
    /// Treat all files as text, disabling binary attributes and detection
    /// </summary>
    DiffOptionForceText = (1 << 20),

    /// <summary>
    /// Treat all files as binary, disabling text diffs
    /// </summary>
    DiffOptionForceBinary = (1 << 21),

    /// <summary>
    /// Ignore all whitespace
    /// </summary>
    DiffOptionIgnoreWhitespace = (1 << 22),

    /// <summary>
    /// Ignore changes in amount of whitespace
    /// </summary>
    DiffOptionIgnoreWhitespaceChange = (1 << 23),

    /// <summary>
    /// Ignore whitespace at end of line
    /// </summary>
    DiffOptionIgnoreWhitespaceEol = (1 << 24),

    /// <summary>
    /// When generating patch text, include the content of untracked
    /// files.  This automatically turns on GIT_DIFF_INCLUDE_UNTRACKED but
    /// it does not turn on GIT_DIFF_RECURSE_UNTRACKED_DIRS.  Add that
    /// flag if you want the content of every single UNTRACKED file.
    /// </summary>
    DiffOptionShowUntrackedContent = (1 << 25),

    /// <summary>
    /// When generating output, include the names of unmodified files if
    /// they are included in the git_diff.  Normally these are skipped in
    /// the formats that list files (e.g. name-only, name-status, raw).
    /// Even with this, these will not be included in patch format.
    /// </summary>
    DiffOptionShowUnmodified = (1 << 26),

    /// <summary>
    /// Use the "patience diff" algorithm
    /// </summary>
    DiffOptionPatience = (1 << 28),

    /// <summary>
    /// Take extra time to find minimal diff
    /// </summary>
    DiffOptionMinimal = (1 << 29),

    /// <summary>
    /// Include the necessary deflate / delta information so that `git-apply`
    /// can apply given diff information to binary files.
    /// </summary>
    DiffOptionShowBinary = (1 << 30),
};
Q_DECLARE_FLAGS(DiffOptionFlags, DiffOptionFlag)

enum ResetMode
{
    /// <summary>
    /// Moves the branch pointed to by HEAD to the specified commit object.
    /// </summary>
    ResetSoft = 1,

    /// <summary>
    /// Moves the branch pointed to by HEAD to the specified commit object and resets the index
    /// to the tree recorded by the commit.
    /// </summary>
    ResetMixed,

    /// <summary>
    /// Moves the branch pointed to by HEAD to the specified commit object, resets the index
    /// to the tree recorded by the commit and updates the working directory to match the content
    /// of the index.
    /// </summary>
    ResetHard,
};

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

QString getChangeKindString(ChangeKind value);
ChangeKind getChangeKind(const QString& value);
QList<ChangeKind> getChangeKindValues();

} // namespace GIT

Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffDeltaFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::FileStatuses)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::GitDiffFindFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffOptionFlags)

#endif // GITTYPES_H
