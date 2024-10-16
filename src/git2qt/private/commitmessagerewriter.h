#ifndef COMMITMESSAGEREWRITER_H
#define COMMITMESSAGEREWRITER_H
#include "rewritehistoryoptions.h"

namespace GIT {

class CommitMessageRewriter : public  RewriteHistoryOptions
{
public:
    CommitMessageRewriter(const QString& message) :
        RewriteHistoryOptions(),
        _message(message) {}

    virtual bool rewriteCommitHeader(const Commit& commit, CommitRewriteInfo& rewriteInfo) override;

private:
    QString _message;
};

} // namespace

#endif // COMMITMESSAGEREWRITER_H
