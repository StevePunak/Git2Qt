#ifndef GITTYPES_H
#define GITTYPES_H
#include <Kanoop/kanoopcommon.h>

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
class ChangeKind {
public:
    enum Kind
    {
        /// <summary>
        /// No changes detected.
        /// </summary>
        Unmodified = 0,

        /// <summary>
        /// The file was added.
        /// </summary>
        Added = 1,

        /// <summary>
        /// The file was deleted.
        /// </summary>
        Deleted = 2,

        /// <summary>
        /// The file content was modified.
        /// </summary>
        Modified = 3,

        /// <summary>
        /// The file was renamed.
        /// </summary>
        Renamed = 4,

        /// <summary>
        /// The file was copied.
        /// </summary>
        Copied = 5,

        /// <summary>
        /// The file is ignored in the workdir.
        /// </summary>
        Ignored = 6,

        /// <summary>
        /// The file is untracked in the workdir.
        /// </summary>
        Untracked = 7,

        /// <summary>
        /// The type (i.e. regular file, symlink, submodule, ...)
        /// of the file was changed.
        /// </summary>
        TypeChanged = 8,

        /// <summary>
        /// Entry is unreadable.
        /// </summary>
        Unreadable = 9,

        /// <summary>
        /// Entry is currently in conflict.
        /// </summary>
        Conflicted = 10,
    };
};

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
    GIT_DELTA_UNMODIFIED = 0,  /**< no changes */
    GIT_DELTA_ADDED = 1,	   /**< entry does not exist in old version */
    GIT_DELTA_DELETED = 2,	   /**< entry does not exist in new version */
    GIT_DELTA_MODIFIED = 3,    /**< entry content changed between old and new */
    GIT_DELTA_RENAMED = 4,     /**< entry was renamed between old and new */
    GIT_DELTA_COPIED = 5,      /**< entry was copied from another old entry */
    GIT_DELTA_IGNORED = 6,     /**< entry is ignored item in workdir */
    GIT_DELTA_UNTRACKED = 7,   /**< entry is untracked item in workdir */
    GIT_DELTA_TYPECHANGE = 8,  /**< type of entry changed between old and new */
    GIT_DELTA_UNREADABLE = 9,  /**< entry is unreadable */
    GIT_DELTA_CONFLICTED = 10  /**< entry in the index is conflicted */
};


QString getFileStatusString(FileStatus value);
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

} // namespace GIT

Q_DECLARE_OPERATORS_FOR_FLAGS(GIT::DiffDeltaFlags)

#endif // GITTYPES_H
