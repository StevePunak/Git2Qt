#ifndef ENUMSTRINGS_H
#define ENUMSTRINGS_H
#include <Kanoop/git/gittypes.h>
#include <Qt>

namespace GIT {

class EnumStrings
{
public:
    static QString getFileStatusString(FileStatuses value);
    static FileStatus getFileStatus(const QString& value) { return _FileStatusToStringMap.getType(value, FileStatus::Nonexistent); }
    static QList<FileStatus> getFileStatusValues() { return _FileStatusToStringMap.getTypes(); }

    static QString getConfigurationLevelString(ConfigurationLevel value) { return _ConfigurationLevelToStringMap.getString(value); }
    static ConfigurationLevel getConfigurationLevel(const QString& value) { return _ConfigurationLevelToStringMap.getType(value, ConfigurationLevel::Local); }
    static QList<ConfigurationLevel> getConfigurationLevelValues() { return _ConfigurationLevelToStringMap.getTypes(); }

    static QString getDeltaTypeString(DeltaType value) { return _DeltaTypeToStringMap.getString(value); }
    static DeltaType getDeltaType(const QString& value) { return _DeltaTypeToStringMap.getType(value, DeltaUnmodified); }
    static QList<DeltaType> getDeltaTypeValues() { return _DeltaTypeToStringMap.getTypes(); }

    static QString getDiffDeltaFlagString(DiffDeltaFlag value) { return _DiffDeltaFlagToStringMap.getString(value); }
    static DiffDeltaFlag getDiffDeltaFlag(const QString& value) { return _DiffDeltaFlagToStringMap.getType(value, (DiffDeltaFlag)0); }
    static QList<DiffDeltaFlag> getDiffDeltaFlagValues() { return _DiffDeltaFlagToStringMap.getTypes(); }

    static QString getReferenceTypeString(ReferenceType value) { return _ReferenceTypeToStringMap.getString(value); }
    static ReferenceType getReferenceType(const QString& value) { return _ReferenceTypeToStringMap.getType(value, UnknownReferenceType); }
    static QList<ReferenceType> getReferenceTypeValues() { return _ReferenceTypeToStringMap.getTypes(); }

    static QString getObjectTypeString(ObjectType value) { return _ObjectTypeToStringMap.getString(value); }
    static ObjectType getObjectType(const QString& value) { return _ObjectTypeToStringMap.getType(value, ObjectTypeInvalid); }
    static QList<ObjectType> getObjectTypeValues() { return _ObjectTypeToStringMap.getTypes(); }

    static QString getModeString(Mode value) { return _ModeToStringMap.getString(value); }
    static Mode getMode(const QString& value) { return _ModeToStringMap.getType(value, NonexistentFile); }
    static QList<Mode> getModeValues() { return _ModeToStringMap.getTypes(); }

    static QString getStageLevelString(StageLevel value) { return _StageLevelToStringMap.getString(value); }
    static StageLevel getStageLevel(const QString& value) { return _StageLevelToStringMap.getType(value); }
    static QList<StageLevel> getStageLevelValues() { return _StageLevelToStringMap.getTypes(); }

    static QString getTreeEntryTargetTypeString(TreeEntryTargetType value) { return _TreeEntryTargetTypeToStringMap.getString(value); }
    static TreeEntryTargetType getTreeEntryTargetType(const QString& value) { return _TreeEntryTargetTypeToStringMap.getType(value); }
    static QList<TreeEntryTargetType> getTreeEntryTargetTypeValues() { return _TreeEntryTargetTypeToStringMap.getTypes(); }

    static QString getBranchTypeString(BranchType value) { return _BranchTypeToStringMap.getString(value); }
    static BranchType getBranchType(const QString& value) { return _BranchTypeToStringMap.getType(value); }
    static QList<BranchType> getBranchTypeValues() { return _BranchTypeToStringMap.getTypes(); }

private:
    class FileStatusToStringMap : public KANOOP::EnumToStringMap<FileStatus>
    {
    public:
        FileStatusToStringMap()
        {
            insert(Nonexistent,         "nonexistent");
            insert(Unaltered,           "unaltered");
            insert(NewInIndex,          "added");
            insert(ModifiedInIndex,     "staged");
            insert(DeletedFromIndex,    "removed");
            insert(RenamedInIndex,      "renamed_in_index");
            insert(TypeChangeInIndex,   "type_changed_in_index");
            insert(NewInWorkdir,        "untracked");
            insert(ModifiedInWorkdir,   "modified");
            insert(DeletedFromWorkdir,  "missing");
            insert(TypeChangeInWorkdir, "type_changed_in_workdir");
            insert(RenamedInWorkdir,    "renamed_in_workdir");
            insert(Unreadable,          "unreadable");
            insert(Ignored,             "ignored");
            insert(Conflicted,          "conflict");
        }
    };

    class ConfigurationLevelToStringMap : public KANOOP::EnumToStringMap<ConfigurationLevel>
    {
    public:
        ConfigurationLevelToStringMap()
        {
            insert(Local,       "Local");
            insert(Global,  "Global");
            insert(Xdg, "Xdg");
            insert(System,  "System");
            insert(ProgramData, "ProgramData");
        }
    };

    class DeltaTypeToStringMap : public KANOOP::EnumToStringMap<DeltaType>
    {
    public:
        DeltaTypeToStringMap()
        {
            insert(DeltaUnmodified,        "Unmodified");
            insert(DeltaAdded,             "Added");
            insert(DeltaDeleted,           "Deleted");
            insert(DeltaModified,          "Modified");
            insert(DeltaRenamed,           "Renamed");
            insert(DeltaCopied,            "Copied");
            insert(DeltaIgnored,           "Ignored");
            insert(DeltaUntracked,         "Untracked");
            insert(DeltaTypeCange,        "Typechange");
            insert(DeltaUnreadable,        "Unreadable");
            insert(DeltaConflicted,        "Conflicted");
        }
    };

    class DiffDeltaFlagToStringMap : public KANOOP::EnumToStringMap<DiffDeltaFlag>
    {
    public:
        DiffDeltaFlagToStringMap()
        {
            insert(GIT_DIFF_FLAG_BINARY,            "Binary");
            insert(GIT_DIFF_FLAG_NOT_BINARY,        "NotBinary");
            insert(GIT_DIFF_FLAG_VALID_ID,          "ValidId");
            insert(GIT_DIFF_FLAG_EXISTS,            "Exists");
            insert(GIT_DIFF_FLAG_VALID_SIZE,        "ValidSize");
        }
    };

    class ReferenceTypeToStringMap : public KANOOP::EnumToStringMap<ReferenceType>
    {
    public:
        ReferenceTypeToStringMap()
        {
            insert(UnknownReferenceType,    "UnknownReferenceType");
            insert(SymbolicReferenceType,   "symbolic");
            insert(DirectReferenceType,     "direct");
        }
    };

    class ObjectTypeToStringMap : public KANOOP::EnumToStringMap<ObjectType>
    {
    public:
        ObjectTypeToStringMap()
        {
            insert(ObjectTypeAny,           "Any");
            insert(ObjectTypeInvalid,       "Invalid");
            insert(ObjectTypeCommit,        "Commit");
            insert(ObjectTypeTree,          "Tree");
            insert(ObjectTypeBlob,          "Blob");
            insert(ObjectTypeTag,           "Tag");
            insert(ObjectTypeDelta,         "Delta");
            insert(ObjectTypeRefDelta,      "RefDelta");
        }
    };

    class ModeToStringMap : public KANOOP::EnumToStringMap<Mode>
    {
    public:
        ModeToStringMap()
        {
            insert(NonexistentFile,                 "NonexistentFile");
            insert(Directory,                       "Directory");
            insert(NonExecutableFile,               "NonExecutableFile");
            insert(NonExecutableGroupWritableFile,  "NonExecutableGroupWritableFile");
            insert(ExecutableFile,                  "ExecutableFile");
            insert(SymbolicLink,                    "SymbolicLink");
            insert(GitLink,                         "GitLink");
        }
    };

    class StageLevelToStringMap : public KANOOP::EnumToStringMap<StageLevel>
    {
    public:
        StageLevelToStringMap()
        {
            insert(Staged,      "Staged");
            insert(Ancestor,    "Ancestor");
            insert(Ours,        "Ours");
            insert(Theirs,      "Theirs");
        }
    };

    class TreeEntryTargetTypeToStringMap : public KANOOP::EnumToStringMap<TreeEntryTargetType>
    {
    public:
        TreeEntryTargetTypeToStringMap()
        {
            insert(TreeEntryTargetBlob,         "blob");
            insert(TreeEntryTargetTree,         "tree");
            insert(TreeEntryTargetGitLink,      "gitlink");
        }
    };

    class BranchTypeToStringMap : public KANOOP::EnumToStringMap<BranchType>
    {
    public:
        BranchTypeToStringMap()
        {
            insert(UnknownBranchType,   "Unknown");
            insert(LocalBranch,         "local");
            insert(RemoteBranch,        "remote");
        }
    };

    static const FileStatusToStringMap _FileStatusToStringMap;
    static const ConfigurationLevelToStringMap _ConfigurationLevelToStringMap;
    static const DeltaTypeToStringMap _DeltaTypeToStringMap;
    static const DiffDeltaFlagToStringMap _DiffDeltaFlagToStringMap;
    static const ReferenceTypeToStringMap _ReferenceTypeToStringMap;
    static const ObjectTypeToStringMap _ObjectTypeToStringMap;
    static const ModeToStringMap _ModeToStringMap;
    static const StageLevelToStringMap _StageLevelToStringMap;
    static const TreeEntryTargetTypeToStringMap _TreeEntryTargetTypeToStringMap;
    static const BranchTypeToStringMap _BranchTypeToStringMap;
};

} // namespace GIT

#endif // ENUMSTRINGS_H
