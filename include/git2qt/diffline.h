/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_line object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFLINE_H
#define DIFFLINE_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>

#include <QList>

namespace GIT {

class GIT2QT_EXPORT DiffLine
{
public:
    DiffLine() {}
    DiffLine(const git_diff_line* line);

    QChar origin() const { return _origin; }
    int oldLineNumber() const { return _oldLineNumber; }
    int newLineNumber() const { return _newLineNumber; }
    int lineCount() const { return _lineCount; }
    int64_t contentOffset() const { return _contentOffset; }
    QByteArray content() const { return _content; }

    bool isValid() const { return _origin.isNull() == false; }
    QString toString() const;

    class List : public QList<DiffLine> {};

private:
    QChar _origin;
    int _oldLineNumber = 0;
    int _newLineNumber = 0;
    int _lineCount = 0;
    int64_t _contentOffset = 0;
    QByteArray _content;
};

} // namespace GIT

#endif // DIFFLINE_H
