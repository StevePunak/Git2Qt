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

QString getDeltaTypeString(DeltaType value)
{
    return EnumStrings::getDeltaTypeString(value);
}

DeltaType getDeltaType(const QString& value)
{
    return EnumStrings::getDeltaType(value);
}

QList<DeltaType> getDeltaTypeValues()
{
    return EnumStrings::getDeltaTypeValues();
}

QString getDiffDeltaFlagString(DiffDeltaFlag value)
{
    return EnumStrings::getDiffDeltaFlagString(value);
}

DiffDeltaFlag getDiffDeltaFlag(const QString& value)
{
    return EnumStrings::getDiffDeltaFlag(value);
}

QList<DiffDeltaFlag> getDiffDeltaFlagValues()
{
    return EnumStrings::getDiffDeltaFlagValues();
}

QString getReferenceTypeString(ReferenceType value)
{
    return EnumStrings::getReferenceTypeString(value);
}

ReferenceType getReferenceType(const QString& value)
{
    return EnumStrings::getReferenceType(value);
}

QList<ReferenceType> getReferenceTypeValues()
{
    return EnumStrings::getReferenceTypeValues();
}

}   // namespace
