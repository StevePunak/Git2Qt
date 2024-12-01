#ifndef COMMITREWRITEINFO_H
#define COMMITREWRITEINFO_H
#include <git2qt/signature.h>

namespace GIT {

class Commit;

class CommitRewriteInfo
{
public:
    CommitRewriteInfo() {}

    CommitRewriteInfo(const Signature& author, const Signature& committer, const QString& message) :
        _author(author), _committer(committer), _message(message) {}

    static CommitRewriteInfo fromCommit(const Commit& commit);

    Signature author() const { return _author; }
    void setAuthor(const Signature& value) { _author = value; }

    Signature committer() const { return _committer; }
    void setCommitter(const Signature& value) { _committer = value; }

    QString message() const { return _message; }
    void setMessage(const QString& value) { _message = value; }

private:
    Signature _author;
    Signature _committer;
    QString _message;
};

} // namespace GIT

#endif // COMMITREWRITEINFO_H
