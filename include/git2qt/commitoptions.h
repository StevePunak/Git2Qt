/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class provides the available commit options.
 * It is a port of the CommitOptions class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef COMMITOPTIONS_H
#define COMMITOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2qt/declspec.h>

#include <QChar>

namespace GIT {

class GIT2QT_EXPORT CommitOptions
{
public:
    CommitOptions() {}

    bool amendPreviousCommit() const { return _amendPreviousCommit; }
    void setAmendPreviousCommit(bool value) { _amendPreviousCommit = value; }

    bool allowEmptyCommit() const { return _allowEmptyCommit; }
    void setAllowEmptyCommit(bool value) { _allowEmptyCommit = value; }

    bool prettifyMessage() const { return _prettifyMessage; }
    void setPrettifyMessage(bool value) { _prettifyMessage = value; }

    QChar commentaryChar() const { return _commentaryChar; }
    void setCommentaryChar(const QChar& value) { _commentaryChar = value; }

private:
    bool _amendPreviousCommit = false;
    bool _allowEmptyCommit = false;
    bool _prettifyMessage = true;
    QChar _commentaryChar;
};

} // namespace GIT

#endif // COMMITOPTIONS_H
