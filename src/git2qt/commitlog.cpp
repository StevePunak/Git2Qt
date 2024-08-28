#include "commitlog.h"

#include <gitexception.h>
#include <repository.h>
#include <utility.h>

using namespace GIT;

CommitLog::CommitLog(Repository* repo, const CommitFilter& filter) :
    GitEntity(CommitLogEntity, repo),
    _filter(filter)
{

}

Commit::List CommitLog::performLookup()
{
    Commit::List commits;
    RevWalkHandle handle = createWalkerHandle();
    try
    {
        throwIfTrue(handle.isNull());
        throwOnError(git_revwalk_sorting(handle.value(), _filter.sortBy()));

        // Include
        ObjectId::List includeObjectIds = _filter.includeReachableFromRefs().objectIds();
        for(const ObjectId& objectId : includeObjectIds) {
            throwOnError(git_revwalk_push(handle.value(), objectId.toNative()));
        }

        // Exclude
        ObjectId::List excludeObjectIds = _filter.excludeReachableFromRefs().objectIds();
        for(const ObjectId& objectId : excludeObjectIds) {
            throwOnError(git_revwalk_hide(handle.value(), objectId.toNative()));
        }

        // First parent only
        if(_filter.firstParentOnly()) {
            throwOnError(git_revwalk_simplify_first_parent(handle.value()));
        }

        ObjectId::List foundObjectIds;
        git_oid oid;
        int res;
        while((res = git_revwalk_next(&oid, handle.value())) == 0) {
            foundObjectIds.append(oid);
        }
        throwIfFalse(res == GIT_ITEROVER);

        for(const ObjectId& commitId : foundObjectIds) {
            Commit commit = Commit::lookup(repository(), commitId);
            if(commit.isNull() == false) {
                // logText(LVL_DEBUG, QString("%1 %2 %3")
                //         .arg(commit.objectId().toString())
                //         .arg(Utility::toString(commit.timestamp()))
                //         .arg(commit.message()));
                commits.append(commit);
            }
            else {
                logText(LVL_DEBUG, "IT's null");
            }
        }

        handle.dispose();
    }
    catch(const GitException&)
    {
        commits.clear();
    }
    return commits;
}

void CommitLog::findReachableFrom(const Commit& commit)
{
    Q_UNUSED(commit)
}

RevWalkHandle CommitLog::createWalkerHandle() const
{
    RevWalkHandle result;
    git_revwalk* walker = nullptr;
    if(git_revwalk_new(&walker, repository()->handle().value()) == 0) {
        result = RevWalkHandle(walker);
    }
    return result;
}
