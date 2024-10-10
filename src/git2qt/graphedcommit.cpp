#include "graphedcommit.h"

#include <gitexception.h>
#include <repository.h>
#include <utility.h>

using namespace GIT;

GraphedCommit::GraphedCommit() :
    Commit()
{
}

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

bool GraphedCommit::operator ==(const GraphedCommit& other) const
{
    return Commit::operator ==(other);
}

void GraphedCommit::setLevel(int value)
{
if(objectId().sha().startsWith("d6134")) {
    logText(LVL_DEBUG, "Ehere");
}
    _level = value;
}

int GraphedCommit::distanceAhead(const Commit& other) const
{
    int result = 0;

    try
    {
        size_t ahead, behind;
        if(getDistance(repository(), objectId(), other.objectId(), &ahead, &behind) == false) {
            throw GitException("failed to find distance");
        }
        result = ahead;
    }
    catch(const GitException& e)
    {
        logText(LVL_ERROR, e.message());
    }
    return result;
}

int GraphedCommit::distanceBehind(const Commit& other) const
{
    int result = 0;

    try
    {
        size_t ahead, behind;
        if(getDistance(repository(), objectId(), other.objectId(), &ahead, &behind) == false) {
            throw GitException("failed to find distance");
        }
        result = behind;
    }
    catch(const GitException& e)
    {
        logText(LVL_ERROR, e.message());
    }
    return result;
}

QString GraphedCommit::toString() const
{
    QString text = QString("[name: %1  lvl: %2  head: %3  parents: %4]")
                   .arg(_friendlyBranchName)
                   .arg(_level)
                   .arg(Utility::toString(_head))
                   .arg(_parentObjectIds.count());
    return text;
}

bool GraphedCommit::getDistance(Repository* repo, const ObjectId& local, const ObjectId& upstream, size_t* ahead, size_t* behind)
{
    bool result = false;
    try
    {
        throwOnError(repo, git_graph_ahead_behind(ahead, behind, repo->handle().value(), local.toNative(), upstream.toNative()));
        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

// -------------------------------- GraphedCommit::List --------------------------------

GraphedCommit::List GraphedCommit::List::findChildren(const GraphedCommit& of) const
{
    List result;
    for(const GraphedCommit& commit : *this) {
        if(of.isStashBase() && commit.isStash() && of.stashBaseOf() == commit.objectId()) {
            result.append(commit);
        }
        else if(commit.parents().objectIds().contains(of.objectId())) {
            result.append(commit);
        }
    }
    return result;
}

