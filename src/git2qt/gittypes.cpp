#include "gittypes.h"

#include <enumstrings.h>

namespace GIT {

QString getFileStatusString(FileStatuses value)
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

QString getObjectTypeString(ObjectType value)
{
    return EnumStrings::getObjectTypeString(value);
}

ObjectType getObjectType(const QString& value)
{
    return EnumStrings::getObjectType(value);
}

QList<ObjectType> getObjectTypeValues()
{
    return EnumStrings::getObjectTypeValues();
}

QString getModeString(Mode value)
{
    return EnumStrings::getModeString(value);
}

Mode getMode(const QString& value)
{
    return EnumStrings::getMode(value);
}

QList<Mode> getModeValues()
{
    return EnumStrings::getModeValues();
}

QString getStageLevelString(StageLevel value)
{
    return EnumStrings::getStageLevelString(value);
}

StageLevel getStageLevel(const QString& value)
{
    return EnumStrings::getStageLevel(value);
}

QList<StageLevel> getStageLevelValues()
{
    return EnumStrings::getStageLevelValues();
}

QString getTreeEntryTargetTypeString(TreeEntryTargetType value)
{
    return EnumStrings::getTreeEntryTargetTypeString(value);
}

TreeEntryTargetType getTreeEntryTargetType(const QString& value)
{
    return EnumStrings::getTreeEntryTargetType(value);
}

QList<TreeEntryTargetType> getTreeEntryTargetTypeValues()
{
    return EnumStrings::getTreeEntryTargetTypeValues();
}

QString getBranchTypeString(BranchType value)
{
    return EnumStrings::getBranchTypeString(value);
}

BranchType getBranchType(const QString& value)
{
    return EnumStrings::getBranchType(value);
}

QList<BranchType> getBranchTypeValues()
{
    return EnumStrings::getBranchTypeValues();
}

}   // namespace
