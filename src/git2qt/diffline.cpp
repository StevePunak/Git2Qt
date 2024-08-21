#include "diffline.h"

using namespace GIT;

DiffLine::DiffLine(const git_diff_line* line) :
    _origin(line->origin),
    _oldLineNumber(line->old_lineno),
    _newLineNumber(line->new_lineno),
    _lineCount(line->num_lines),
    _contentOffset(line->content_offset),
    _content(line->content, line->content_len)
{
}

QString DiffLine::toString() const
{
    QString output = QString("%1  old line: %2  new line: %3  line count: %4  offset: %5")
                         .arg(_origin)
                         .arg(_oldLineNumber).arg(_newLineNumber)
                         .arg(_lineCount).arg(_contentOffset);
    return output;
}
