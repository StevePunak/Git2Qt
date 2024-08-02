#include "diffhunk.h"

using namespace GIT;

DiffHunk::DiffHunk(const git_diff_hunk* hunk) :
    _oldStart(hunk->old_start),
    _oldLines(hunk->old_lines),
    _newStart(hunk->new_start),
    _newLines(hunk->new_lines),
    _header(hunk->header)
{
}

bool DiffHunk::operator ==(const DiffHunk& other) const
{
    return _oldStart == other._oldStart &&
        _oldLines == other._oldLines &&
        _newStart == other._newStart &&
        _newLines == other._newLines &&
        _header == other._header;
}

QString DiffHunk::toString() const
{
    QString output = QString("old start: %1  old lines: %2  new start: %3  new lines: %4  hdr: %5")
                         .arg(_oldStart).arg(_oldLines)
                         .arg(_newStart).arg(_newLines)
                         .arg(_header);
    return output;
}
