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
#include <git2qt/declspec.h>
#include <git2.h>

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

enum StashModifier
{
    /// <summary>
    /// Default
    /// </summary>
    StashModifierDefault = 0,

    /// <summary>
    /// All changes already added to the index
    /// are left intact in the working directory
    /// </summary>
    StashModifierKeepIndex = (1 << 0),

    /// <summary>
    /// All untracked files are also stashed and then
    /// cleaned up from the working directory
    /// </summary>
    StashModifierIncludeUntracked = (1 << 1),

    /// <summary>
    /// All ignored files are also stashed and then
    /// cleaned up from the working directory
    /// </summary>
    StashModifierIncludeIgnored = (1 << 2),
};
Q_DECLARE_FLAGS(StashModifiers, StashModifier)

enum StashApplyModifier
{
    /// <summary>
    /// Default. Will apply the stash and result in an index with conflicts
    /// if any arise.
    /// </summary>
    StashApplyDefault = 0,

    /// <summary>
    /// In case any conflicts arise, this will not apply the stash.
    /// </summary>
    StashApplyReinstateIndex = (1 << 0),
};
Q_DECLARE_FLAGS(StashApplyModifiers, StashApplyModifier)

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
    StatusShowIndexAndWorkDir = 0,
    StatusShowIndexOnly = 1,
    StatusShowWorkDirOnly = 2,
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
    UnknownReferenceType    = 0x00,
    DirectReferenceType     = 0x01,
    SymbolicReferenceType   = 0x02,
    AllReferenceTypes = (DirectReferenceType |  SymbolicReferenceType),
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

enum GitEntityType {
    UnknownGitEntityType = 0,

    BlobEntity,
    BranchCollectionEntity,
    BranchEntity,
    CommitEntity,
    CommitLogEntity,
    ConfigurationEntity,
    DiffEntity,
    GraphBuilderEntity,
    GraphedCommitEntity,
    HistoryRewriterEntity,
    IndexEntity,
    NetworkEntity,
    ObjectDatabaseEntity,
    ReferenceCollectionEntity,
    ReferenceEntity,
    ReflogCollectionEntity,
    ReflogEntity,
    RemoteCollectionEntity,
    RemoteEntity,
    RepositoryEntity,
    RepositoryInfoEntity,
    SignatureEntity,
    StashCollectionEntity,
    StashEntity,
    SubmoduleCollectionEntity,
    SubmoduleEntity,
    TagAnnotatedEntity,
    TagCollectionEntity,
    TagLightweightEntity,
    TreeEntity,
    TreeEntryEntity,
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
    UnknownBranchType   = 0x0,
    LocalBranch         = 0x1,
    RemoteBranch        = 0x2,
    BranchTypeAll = LocalBranch | RemoteBranch
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

enum CommitSortStrategy
{
    /// <summary>
    /// Sort the commits in no particular ordering;
    /// this sorting is arbitrary, implementation-specific
    /// and subject to change at any time.
    /// </summary>
    SortStrategyNone = 0,

    /// <summary>
    /// Sort the commits in topological order
    /// (parents before children); this sorting mode
    /// can be combined with time sorting.
    /// </summary>
    SortStrategyTopological = (1 << 0),

    /// <summary>
    /// Sort the commits by commit time;
    /// this sorting mode can be combined with
    /// topological sorting.
    /// </summary>
    SortStrategyTime = (1 << 1),

    /// <summary>
    /// Iterate through the commits in reverse
    /// order; this sorting mode can be combined with
    /// any of the above.
    /// </summary>
    SortStrategyReverse = (1 << 2)
};
Q_DECLARE_FLAGS(CommitSortStrategies, CommitSortStrategy)

enum MergeBaseFindingStrategy
{
    /// <summary>
    /// Compute the best common ancestor between some commits to use in a three-way merge.
    /// <para>
    /// When more than two commits are provided, the computation is performed between the first commit and a hypothetical merge commit across all the remaining commits.
    /// </para>
    /// </summary>
    MergeBaseFindStandard,
    /// <summary>
    /// Compute the best common ancestor of all supplied commits, in preparation for an n-way merge.
    /// </summary>
    MergeBaseFindOctopus,
};

enum GraphItemType
{
    NoGraphItem         = 0x00000000,
    VerticalUp          = 0x00000001,
    VerticalDown        = 0x00000002,
    TopTermination      = 0x00000004,
    BottomTermination   = 0x00000008,
    CommitDot           = 0x00000010,
    MergeDot            = 0x00000020,
    UpToRight           = 0x00000040,
    UpToLeft            = 0x00000080,
    DownToRight         = 0x00000100,
    DownToLeft          = 0x00000200,
    RightThenUp         = 0x00000400,
    RightThenDown       = 0x00000800,
    LeftThenUp          = 0x00001000,
    LeftThenDown        = 0x00002000,
    HorizontalLeft      = 0x00004000,
    HorizontalRight     = 0x00008000,

    AnyHorizontal       =   (UpToRight | UpToLeft |
                            DownToRight | DownToLeft |
                            RightThenUp | RightThenDown |
                            LeftThenUp | LeftThenDown |
                            HorizontalLeft | HorizontalRight),

};
Q_DECLARE_FLAGS(GraphItemTypes, GraphItemType)

enum TagFetchMode {
    /**
     * Use the setting from the configuration.
     */
    RemoteDownloadTagsUnspecified = 0,
    /**
     * Ask the server for tags pointing to objects we're already
     * downloading.
     */
    RemoteDownloadTagsAuto,
    /**
     * Don't ask for any tags beyond the refspecs.
     */
    RemoteDownloadTagsNone,
    /**
     * Ask for the all the tags.
     */
    RemoteDownloadTagsAll
};

enum FastForwardStrategy
{
    /**
     * No configuration was found that suggests a preferred behavior for
     * merge.
     */
    FastForwardDefault = 0,

    /**
     * There is a `merge.ff=false` configuration setting, suggesting that
     * the user does not want to allow a fast-forward merge.
     */
    NoFastForward = (1 << 0),

    /**
     * There is a `merge.ff=only` configuration setting, suggesting that
     * the user only wants fast-forward merges.
     */
    FastForwardOnly = (1 << 1)
};
Q_DECLARE_FLAGS(FastForwardStrategies, FastForwardStrategy)

enum MergeAnalysis
{
    /// <summary>
    /// No merge is possible.  (Unused.)
    /// </summary>
    MergeAnalysisNone = 0,

    /// <summary>
    /// A "normal" merge; both HEAD and the given merge input have diverged
    /// from their common ancestor.  The divergent commits must be merged.
    /// </summary>
    MergeAnalysisNormal = (1 << 0),

    /// <summary>
    /// All given merge inputs are reachable from HEAD, meaning the
    /// repository is up-to-date and no merge needs to be performed.
    /// </summary>
    MergeAnalysisUpToDate = (1 << 1),

    /// <summary>
    /// The given merge input is a fast-forward from HEAD and no merge
    /// needs to be performed.  Instead, the client can check out the
    /// given merge input.
    /// </summary>
    MergeAnalysisFastforward = (1 << 2),

    /// <summary>
    /// The HEAD of the current repository is "unborn" and does not point to
    /// a valid commit.  No merge can be performed, but the caller may wish
    /// to simply set HEAD to the target commit(s).
    /// </summary>
    MergeAnalysisUnborn = (1 << 3),
};
Q_DECLARE_FLAGS(MergeAnalyses, MergeAnalysis)

enum MergePreference
{
    /// <summary>
    /// No configuration was found that suggests a preferred behavior for
    /// merge.
    /// </summary>
    MergePreferenceNone = 0,

    /// <summary>
    /// There is a `merge.ff=false` configuration setting, suggesting that
    /// the user does not want to allow a fast-forward merge.
    /// </summary>
    MergePreferenceNoFastForward = (1 << 0),

    /// <summary>
    /// There is a `merge.ff=only` configuration setting, suggesting that
    /// the user only wants fast-forward merges.
    /// </summary>
    MergePreferenceFastForwardOnly = (1 << 1),
};
Q_DECLARE_FLAGS(MergePreferences, MergePreference)

enum MergeFlag
{
    /// <summary>
    /// No options.
    /// </summary>
    MergeFlagNormal = 0,

    /// <summary>
    /// Detect renames that occur between the common ancestor and the "ours"
    /// side or the common ancestor and the "theirs" side.  This will enable
    /// the ability to merge between a modified and renamed file.
    /// </summary>
    MergeFlagFindRenames = (1 << 0),

    /// <summary>
    /// If a conflict occurs, exit immediately instead of attempting to
    /// continue resolving conflicts.  The merge operation will fail with
    /// GIT_EMERGECONFLICT and no index will be returned.
    ///</summary>
    MergeFlagFailOnConflict = (1 << 1),

    /// <summary>
    /// Do not write the REUC extension on the generated index
    /// </summary>
    MergeFlagSkipReuc = (1 << 2),

    /// <summary>
    /// If the commits being merged have multiple merge bases, do not build
    /// a recursive merge base (by merging the multiple merge bases),
    /// instead simply use the first base.  This flag provides a similar
    /// merge base to `git-merge-resolve`.
    /// </summary>
    MergeFlagNoRecursive = (1 << 3),
};
Q_DECLARE_FLAGS(MergeFlags, MergeFlag)

enum MergeFileFlag
{
    /// <summary>
    /// Defaults
    /// </summary>
    MergeFileDefault = 0,

    /// <summary>
    /// Create standard conflicted merge files
    /// </summary>
    MergeFileStyleMerge = (1 << 0),

    /// <summary>
    /// Create diff3-style files
    /// </summary>
    MergeFileStyleDifF3 = (1 << 1),

    /// <summary>
    /// Condense non-alphanumeric regions for simplified diff file
    /// </summary>
    MergeFileSimplifyAlnum = (1 << 2),

    /// <summary>
    /// Ignore all whitespace
    /// </summary>
    MergeFileIgnoreWhitespace = (1 << 3),

    /// <summary>
    /// Ignore changes in amount of whitespace
    /// </summary>
    MergeFileIgnoreWhitespaceChange = (1 << 4),

    /// <summary>
    /// Ignore whitespace at end of line
    /// </summary>
    MergeFileIgnoreWhitespaceEol = (1 << 5),

    /// <summary>
    /// Use the "patience diff" algorithm
    /// </summary>
    MergeFileDiffPatience = (1 << 6),

    /// <summary>
    /// Take extra time to find minimal diff
    /// </summary>
    MergeFileDiffMinimal = (1 << 7),
};
Q_DECLARE_FLAGS(MergeFileFlags, MergeFileFlag)

enum CheckoutNotifyFlag
{

    /// <summary>
    /// No checkout notification.
    /// </summary>
    CheckoutNotifyNone = 0, /* GIT_CHECKOUT_NOTIFY_NONE */

    /// <summary>
    /// Notify on conflicting paths.
    /// </summary>
    CheckoutNotifyConflict = (1 << 0), /* GIT_CHECKOUT_NOTIFY_CONFLICT */

    /// <summary>
    /// Notify about dirty files. These are files that do not need
    /// an update, but no longer match the baseline.
    /// </summary>
    CheckoutNotifyDirty = (1 << 1), /* GIT_CHECKOUT_NOTIFY_DIRTY */

    /// <summary>
    /// Notify for files that will be updated.
    /// </summary>
    CheckoutNotifyUpdated = (1 << 2), /* GIT_CHECKOUT_NOTIFY_UPDATED */

    /// <summary>
    /// Notify for untracked files.
    /// </summary>
    CheckoutNotifyUntracked = (1 << 3), /* GIT_CHECKOUT_NOTIFY_UNTRACKED */

    /// <summary>
    /// Notify about ignored file.
    /// </summary>
    CheckoutNotifyIgnored = (1 << 4), /* GIT_CHECKOUT_NOTIFY_IGNORED */
};
Q_DECLARE_FLAGS(CheckoutNotifyFlags, CheckoutNotifyFlag)

/// <summary>
/// Enum specifying what content checkout should write to disk
/// for conflicts.
/// </summary>
enum CheckoutFileConflictStrategy
{
    /// <summary>
    /// Use the default behavior for handling file conflicts. This is
    /// controlled by the merge.conflictstyle config option, and is "Merge"
    /// if no option is explicitly set.
    /// </summary>
    CheckoutFileConflictNormal,

    /// <summary>
    /// For conflicting files, checkout the "ours" (stage 2)  version of
    /// the file from the index.
    /// </summary>
    CheckoutFileConflictOurs,

    /// <summary>
    /// For conflicting files, checkout the "theirs" (stage 3) version of
    /// the file from the index.
    /// </summary>
    CheckoutFileConflictTheirs,

    /// <summary>
    /// Write normal merge files for conflicts.
    /// </summary>
    CheckoutFileConflictMerge,

    /// <summary>
    /// Write diff3 formated files for conflicts.
    /// </summary>
    CheckoutFileConflictDiff3
};

enum CheckoutStrategy
{
    /// <summary>
    /// Default is a dry run, no actual updates.
    /// </summary>
    CheckoutStrategyNone = GIT_CHECKOUT_NONE,

    /// <summary>
    /// Allow safe updates that cannot overwrite uncommited data.
    /// </summary>
    CheckoutStrategySafe = GIT_CHECKOUT_SAFE,

    /// <summary>
    /// Allow update of entries in working dir that are modified from HEAD.
    /// </summary>
    CheckoutStrategyForce = GIT_CHECKOUT_FORCE,

    /// <summary>
    /// Allow checkout to recreate missing files.
    /// </summary>
    CheckoutStrategyRecreateMissing = GIT_CHECKOUT_RECREATE_MISSING,

    /// <summary>
    /// Allow checkout to make safe updates even if conflicts are found
    /// </summary>
    CheckoutStrategyAllowConflicts = GIT_CHECKOUT_ALLOW_CONFLICTS,

    /// <summary>
    /// Remove untracked files not in index (that are not ignored)
    /// </summary>
    CheckoutStrategyRemoveUntracked = GIT_CHECKOUT_REMOVE_UNTRACKED,

    /// <summary>
    /// Remove ignored files not in index
    /// </summary>
    CheckoutStrategy_REMOVE_IGNORED = GIT_CHECKOUT_REMOVE_IGNORED,

    /// <summary>
    /// Only update existing files, don't create new ones
    /// </summary>
    CheckoutStrategyUpdateOnly = GIT_CHECKOUT_UPDATE_ONLY,

    /// <summary>
    /// Normally checkout updates index entries as it goes; this stops that
    /// Implies `GIT_CHECKOUT_DONT_WRITE_INDEX`.
    /// </summary>
    CheckoutStrategyDontUpdateIndex = GIT_CHECKOUT_DONT_UPDATE_INDEX,

    /// <summary>
    /// Don't refresh index/config/etc before doing checkout
    /// </summary>
    CheckoutStrategyNoRefresh = GIT_CHECKOUT_NO_REFRESH,

    ///Allow checkout to skip unmerged files
    CheckoutStrategySkipUnmerged = GIT_CHECKOUT_SKIP_UNMERGED,

    /// <summary>
    /// For unmerged files, checkout stage 2 from index
    /// </summary>
    CheckoutStrategyUseOurs = GIT_CHECKOUT_USE_OURS,

    /// <summary>
    /// For unmerged files, checkout stage 3 from index
    /// </summary>
    CheckoutStrategyUseTheirs = GIT_CHECKOUT_USE_THEIRS,

    /// <summary>
    /// Treat pathspec as simple list of exact match file paths
    /// </summary>
    CheckoutStrategyDisablePathspecMatch = GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH,

    /// <summary>
    /// Ignore directories in use, they will be left empty
    /// </summary>
    CheckoutStrategySkipLockedDirectories = GIT_CHECKOUT_SKIP_LOCKED_DIRECTORIES,

    /// <summary>
    /// Don't overwrite ignored files that exist in the checkout target
    /// </summary>
    CheckoutStrategyDontOverwriteIgnored = GIT_CHECKOUT_DONT_OVERWRITE_IGNORED,

    /// <summary>
    /// Write normal merge files for conflicts
    /// </summary>
    CheckoutStrategyConflictStyleMerge = GIT_CHECKOUT_CONFLICT_STYLE_MERGE,

    /// <summary>
    /// Include common ancestor data in diff3 format files for conflicts
    /// </summary>
    CheckoutStrategyConflictStyleDiff3 = GIT_CHECKOUT_CONFLICT_STYLE_DIFF3,

    /// <summary>
    /// Don't overwrite existing files or folders
    /// </summary>
    CheckoutStrategyDontRemoveExisting = GIT_CHECKOUT_DONT_REMOVE_EXISTING,

    /// <summary>
    /// Normally checkout writes the index upon completion; this prevents that.
    /// </summary>
    CheckoutStrategyDontWriteIndex = GIT_CHECKOUT_DONT_WRITE_INDEX,

    // THE FOLLOWING OPTIONS ARE NOT YET IMPLEMENTED

    /// <summary>
    /// Recursively checkout submodules with same options (NOT IMPLEMENTED)
    /// </summary>
    CheckoutStrategyUpdateSubmodules = GIT_CHECKOUT_UPDATE_SUBMODULES,

    /// <summary>
    /// Recursively checkout submodules if HEAD moved in super repo (NOT IMPLEMENTED)
    /// </summary>
    CheckoutStrategyUpdateSubmodulesIfChanged = GIT_CHECKOUT_UPDATE_SUBMODULES_IF_CHANGED,
};

GIT2QT_EXPORT QString getFileStatusString(FileStatuses value);
GIT2QT_EXPORT FileStatus getFileStatus(const QString& value);
GIT2QT_EXPORT QList<FileStatus> getFileStatusValues();

GIT2QT_EXPORT QString getConfigurationLevelString(ConfigurationLevel value);
GIT2QT_EXPORT ConfigurationLevel getConfigurationLevel(const QString& value);
GIT2QT_EXPORT QList<ConfigurationLevel> getConfigurationLevelValues();

GIT2QT_EXPORT QString getDeltaTypeString(DeltaType value);
GIT2QT_EXPORT DeltaType getDeltaType(const QString& value);
GIT2QT_EXPORT QList<DeltaType> getDeltaTypeValues();

GIT2QT_EXPORT QString getDiffDeltaFlagString(DiffDeltaFlag value);
GIT2QT_EXPORT DiffDeltaFlag getDiffDeltaFlag(const QString& value);
GIT2QT_EXPORT QList<DiffDeltaFlag> getDiffDeltaFlagValues();

GIT2QT_EXPORT QString getReferenceTypeString(ReferenceType value);
GIT2QT_EXPORT ReferenceType getReferenceType(const QString& value);
GIT2QT_EXPORT QList<ReferenceType> getReferenceTypeValues();

GIT2QT_EXPORT QString getObjectTypeString(ObjectType value);
GIT2QT_EXPORT ObjectType getObjectType(const QString& value);
GIT2QT_EXPORT QList<ObjectType> getObjectTypeValues();

GIT2QT_EXPORT QString getModeString(Mode value);
GIT2QT_EXPORT Mode getMode(const QString& value);
GIT2QT_EXPORT QList<Mode> getModeValues();

GIT2QT_EXPORT QString getStageLevelString(StageLevel value);
GIT2QT_EXPORT StageLevel getStageLevel(const QString& value);
GIT2QT_EXPORT QList<StageLevel> getStageLevelValues();

GIT2QT_EXPORT QString getTreeEntryTargetTypeString(TreeEntryTargetType value);
GIT2QT_EXPORT TreeEntryTargetType getTreeEntryTargetType(const QString& value);
GIT2QT_EXPORT QList<TreeEntryTargetType> getTreeEntryTargetTypeValues();

GIT2QT_EXPORT QString getBranchTypeString(BranchType value);
GIT2QT_EXPORT BranchType getBranchType(const QString& value);
GIT2QT_EXPORT QList<BranchType> getBranchTypeValues();

GIT2QT_EXPORT QString getChangeKindString(ChangeKind value);
GIT2QT_EXPORT ChangeKind getChangeKind(const QString& value);
GIT2QT_EXPORT QList<ChangeKind> getChangeKindValues();

GIT2QT_EXPORT QString getGitEntityTypeString(GitEntityType value);
GIT2QT_EXPORT GitEntityType getGitEntityType(const QString& value);
GIT2QT_EXPORT QList<GitEntityType> getGitEntityTypeValues();

} // namespace GIT

Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffDeltaFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::FileStatuses)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::StashModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::StashApplyModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffModifiers)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::GitDiffFindFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffOptionFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::CommitSortStrategies)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::GraphItemTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::FastForwardStrategies)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::MergeAnalyses)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::MergePreferences)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::MergeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::MergeFileFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::CheckoutNotifyFlags)

#endif // GITTYPES_H
