#include "graphedcommit.h"

#include <gitexception.h>
#include <utility.h>

using namespace GIT;

GraphedCommit::GraphedCommit(const Commit& other) :
    Commit(GraphedCommitEntity, other.repository(), other.objectId())
{
    CommitHandle handle = createHandle();
    try
    {
        throwIfTrue(handle.isNull());
        int count = git_commit_parentcount(handle.value());
        for(int i = 0;i < count;i++) {
            const git_oid* oid = git_commit_parent_id(handle.value(), i);
            throwIfNull(oid);
            _parentObjectIds.append(oid);
        }
    }
    catch(const GitException&)
    {
    }
}

QString GraphedCommit::toString() const
{
    QString text = QString("[name: %1  lvl: %2  head: %3  parents: %4]")
                   .arg(_branchName)
                   .arg(_level)
                   .arg(Utility::toString(_head))
                   .arg(_parentObjectIds.count());
    return text;
}
