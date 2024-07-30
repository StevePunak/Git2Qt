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

};
