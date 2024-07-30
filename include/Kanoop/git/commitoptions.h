#ifndef COMMITOPTIONS_H
#define COMMITOPTIONS_H
#include <Kanoop/git/gittypes.h>

#include <QChar>

namespace GIT {

class CommitOptions
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
