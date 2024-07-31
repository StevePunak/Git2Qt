#include "commit.h"

#include <repository.h>
#include <objectid.h>
#include <tree.h>

using namespace GIT;

Commit::~Commit()
{
}

Commit Commit::lookup(Repository* repo, const ObjectId& objectId)
{
    Commit result(repo);
    git_commit *commit;
    if(git_commit_lookup(&commit, repo->handle().value(), objectId.toNative()) == 0) {
        result = createFromNative(repo, commit);
        git_commit_free(commit);
    }
    return result;
}

ObjectId Commit::treeId() const
{
    ObjectId result;
    git_commit *thisCommit = nullptr;
    if(git_commit_lookup(&thisCommit, _repo->handle().value(), _objectId.toNative()) == 0) {
        const git_oid* oid = git_commit_tree_id(thisCommit);
        result = ObjectId(oid);
    }
    return result;
}

Commit::List Commit::parents() const
{
    Commit::List result = _parents;

    if(result.count() == 0) {
        git_commit *thisCommit;
        if(git_commit_lookup(&thisCommit, _repo->handle().value(), _objectId.toNative()) == 0) {
            int count = git_commit_parentcount(thisCommit);
            for (int i = 0;i < count;i++) {
                git_commit *parent;
                git_commit_parent(&parent, thisCommit, i);
                Commit commit = createFromNative(_repo, parent);
                result.append(commit);
                git_commit_free(parent);
            }
            git_commit_free(thisCommit);
        }
    }
    return result;
}

Commit Commit::createFromNative(Repository* repo, git_commit* commit)
{
    Commit result(repo);
    result._author = Signature(git_commit_author(commit));
    result._committer = Signature(git_commit_committer(commit));
    result._message = git_commit_message(commit);
    result._shortMessage = git_commit_summary(commit);
    result._encoding = git_commit_message_encoding(commit);
    result._timestamp = QDateTime::fromSecsSinceEpoch((int64_t)git_commit_time(commit), Qt::UTC);
    result._objectId = ObjectId(git_commit_id(commit));
    return result;
}
