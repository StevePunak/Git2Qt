#ifndef REWRITEHISTORYOPTIONS_H
#define REWRITEHISTORYOPTIONS_H

#include <git2qt/commit.h>

namespace GIT {

class CommitRewriteInfo;
class Commit;

class RewriteHistoryOptions
{
public:
    RewriteHistoryOptions();

    QString backupRefsNamespace() const { return _backupRefsNamespace; }
    void setBackupRefsNamespace(const QString& value) { _backupRefsNamespace = value; }

    bool prettifyMessages() const { return _prettifyMessages; }
    void setPrettifyMessages(bool value) { _prettifyMessages = value; }

    // virtual functions to be derived to perform rewrites
    virtual bool rewriteCommitHeader(const Commit& commit, CommitRewriteInfo& rewriteInfo) { Q_UNUSED(commit) Q_UNUSED(rewriteInfo)  return false; }

private:
    QString _backupRefsNamespace = "/refs/original";
    bool _prettifyMessages = false;
};

} // namespace GIT

#endif // REWRITEHISTORYOPTIONS_H
