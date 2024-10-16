#include "commitmessagerewriter.h"

using namespace  GIT;

bool CommitMessageRewriter::rewriteCommitHeader(const Commit& commit, CommitRewriteInfo& rewriteInfo)
{
    Q_UNUSED(commit)
    Q_UNUSED(rewriteInfo)
    return false;
}
