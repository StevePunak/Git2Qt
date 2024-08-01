#ifndef ENUMSTRINGS_H
#define ENUMSTRINGS_H
#include <Kanoop/git/gittypes.h>
#include <Qt>

namespace GIT {

class EnumStrings
{
public:
    static QString getFileStatusString(FileStatus value) { return _FileStatusToStringMap.getString(value); }
    static FileStatus getFileStatus(const QString& value) { return _FileStatusToStringMap.getType(value, FileStatus::Nonexistent); }
    static QList<FileStatus> getFileStatusValues() { return _FileStatusToStringMap.getTypes(); }

    static QString getConfigurationLevelString(ConfigurationLevel value) { return _ConfigurationLevelToStringMap.getString(value); }
    static ConfigurationLevel getConfigurationLevel(const QString& value) { return _ConfigurationLevelToStringMap.getType(value, ConfigurationLevel::Local); }
    static QList<ConfigurationLevel> getConfigurationLevelValues() { return _ConfigurationLevelToStringMap.getTypes(); }

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

    static const FileStatusToStringMap _FileStatusToStringMap;
    static const ConfigurationLevelToStringMap _ConfigurationLevelToStringMap;
};

} // namespace GIT

#endif // ENUMSTRINGS_H
