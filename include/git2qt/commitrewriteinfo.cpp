#include "commitrewriteinfo.h"

#include "commit.h"

using namespace GIT;

CommitRewriteInfo CommitRewriteInfo::fromCommit(const Commit& commit)
{
    CommitRewriteInfo info(commit.author(), commit.committer(), commit.message());
    return info;
}
