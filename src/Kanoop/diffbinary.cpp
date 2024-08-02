#include "diffbinary.h"

#include <Kanoop/stringutil.h>

using namespace GIT;

DiffBinary::DiffBinary(const git_diff_binary* binary) :
    _containsData(binary->contains_data == 1),
    _oldFile(&binary->old_file),
    _newFile(&binary->new_file)
{
}

QString DiffBinary::toString() const
{
    QString output = QString("has data: %1  old file: %2  new file: %3")
                         .arg(StringUtil::toString(_containsData))
                         .arg(_oldFile.toString())
                         .arg(_newFile.toString());
    return output;
}
