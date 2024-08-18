/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_line object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFLINE_H
#define DIFFLINE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class DiffLine
{
public:
    DiffLine(const git_diff_line* line);

    QChar origin() const { return _origin; }
    int oldLineNumber() const { return _oldLineNumber; }
    int newLineNumber() const { return _newLineNumber; }
    int lineCount() const { return _lineCount; }
    int64_t contentOffset() const { return _contentOffset; }
    QByteArray content() const { return _content; }

    QString toString() const;

    class List : public QList<DiffLine> {};

private:
    QChar _origin;
    int _oldLineNumber;
    int _newLineNumber;
    int _lineCount;
    int64_t _contentOffset;
    QByteArray _content;
};

} // namespace GIT

#endif // DIFFLINE_H
