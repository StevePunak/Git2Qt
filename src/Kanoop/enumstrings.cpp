#include "enumstrings.h"

#include <QTextStream>

namespace GIT {

const EnumStrings::FileStatusToStringMap EnumStrings::_FileStatusToStringMap;
const EnumStrings::ConfigurationLevelToStringMap EnumStrings::_ConfigurationLevelToStringMap;
const EnumStrings::DeltaTypeToStringMap EnumStrings::_DeltaTypeToStringMap;
const EnumStrings::DiffDeltaFlagToStringMap EnumStrings::_DiffDeltaFlagToStringMap;
const EnumStrings::ReferenceTypeToStringMap EnumStrings::_ReferenceTypeToStringMap;
const EnumStrings::ObjectTypeToStringMap EnumStrings::_ObjectTypeToStringMap;
const EnumStrings::ModeToStringMap EnumStrings::_ModeToStringMap;
const EnumStrings::StageLevelToStringMap EnumStrings::_StageLevelToStringMap;
const EnumStrings::TreeEntryTargetTypeToStringMap EnumStrings::_TreeEntryTargetTypeToStringMap;
const EnumStrings::BranchTypeToStringMap EnumStrings::_BranchTypeToStringMap;


QString EnumStrings::getFileStatusString(FileStatuses value)
{
    QString result;
    QTextStream output(&result);

    QList<FileStatus> statuses = _FileStatusToStringMap.keys();
    for(FileStatus status : statuses) {
        if((value & status) != 0) {
            output << _FileStatusToStringMap.getString(status) << ',';
        }
    }
    if(result.endsWith(',')) {
        result = result.mid(0, result.length() - 1);
    }
    return result;
}

}
