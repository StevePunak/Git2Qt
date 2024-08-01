#include "gittypes.h"

#include <enumstrings.h>

namespace GIT {

QString getFileStatusString(FileStatus value)
{
    return EnumStrings::getFileStatusString(value);
}

FileStatus getFileStatus(const QString& value)
{
    return EnumStrings::getFileStatus(value);
}

QList<FileStatus> getFileStatusValues()
{
    return EnumStrings::getFileStatusValues();
}

QString getConfigurationLevelString(ConfigurationLevel value)
{
    return EnumStrings::getConfigurationLevelString(value);
}

ConfigurationLevel getConfigurationLevel(const QString& value)
{
    return EnumStrings::getConfigurationLevel(value);
}

QList<ConfigurationLevel> getConfigurationLevelValues()
{
    return EnumStrings::getConfigurationLevelValues();
}

}   // namespace
