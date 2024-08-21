#include "diffdelta.h"

using namespace GIT;

DiffDelta::DiffDelta(const git_diff_delta* delta) :
    _status((DeltaType)delta->status),
    _flags(delta->flags),
    _similarity(delta->similarity),
    _fileCount(delta->nfiles),
    _oldFile(&delta->old_file),
    _newFile(&delta->new_file)

{}

QString DiffDelta::toString() const
{
    QString output = QString("Old File: %1  New File: %2  status: %3  flags: 0x%4  sim: %5  count: %6")
                         .arg(_oldFile.path()).arg(_newFile.path())
                         .arg(getDeltaTypeString(_status))
                         .arg(_flags, 0, 16)
                         .arg(_similarity)
                         .arg(_fileCount);
    return output;
}
