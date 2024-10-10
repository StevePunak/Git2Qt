#include "graphbuildercommit.h"

#include "graphbuilder.h"

#include <gitexception.h>
#include <repository.h>
#include <stash.h>

using namespace GIT;

GraphBuilderCommit::GraphBuilderCommit() :
    GraphedCommit()
{
}

GraphBuilderCommit::GraphBuilderCommit(const Commit& other) :
    GraphedCommit(other)
{
}

GraphBuilderCommit::~GraphBuilderCommit()
{
}

void GraphBuilderCommit::copyBranchInformation(const GraphBuilderCommit* from)
{
    setBranchName(from->branchName());
    setFriendlyBranchName(from->friendlyBranchName());
}

GraphedCommit GraphBuilderCommit::toGraphedCommit() const
{
    GraphedCommit commit(*this);
    commit.setChildObjectIds(_childCommits.objectIds());
    commit.setParentObjectIds(_parentCommits.objectIds());
    return commit;
}

// -------------------------------- GraphBuilderCommit::PtrList --------------------------------

void GraphBuilderCommit::PtrList::detectStashes(Repository* repo)
{
    Map map(*this);
    Stash::List stashes = repo->stashes();
    PtrList deleteList;
    for(const Stash& stash : stashes) {
        GraphBuilderCommit* commit;
        if((commit = map.value(stash.index().objectId())) != nullptr && deleteList.contains(commit) == false) {
            deleteList.append(commit);
        }
        if((commit = map.value(stash.untracked().objectId())) != nullptr && deleteList.contains(commit) == false) {
            deleteList.append(commit);
        }
        if((commit = map.value(stash.untracked().objectId())) != nullptr) {
            commit->setStash(true);
        }
    }

    for(GraphBuilderCommit* commit : deleteList) {
        removeAll(commit);
    }
    qDeleteAll(deleteList);

    // rebuild indexes
    for(int index = 0;index < count();index++) {
        GraphBuilderCommit* commit = at(index);
        commit->setIndex(index);
    }
}

void GraphBuilderCommit::PtrList::resolveParentsAndChildren()
{
    Map map(*this);

    for(GraphBuilderCommit* commit : *this) {
        for(const ObjectId& parentId : commit->parentObjectIds()) {
            GraphBuilderCommit* parent = map.value(parentId);
            if(parent != nullptr) {
                parent->childCommitsRef().append(commit);
                commit->parentCommitsRef().append(parent);
            }
            else {
                Log::logText(LVL_WARNING, "Failed to find child in map");
            }
        }
    }
}

void GraphBuilderCommit::PtrList::resolveMergesAndStashes(Repository* repo)
{
    Map map(*this);

    Stash::List stashes = repo->stashes();

    try
    {
        for(GraphBuilderCommit* commit : *this) {
            if(commit->parents().count() > 1) {
                /**
                 * This is a merge or a stash
                 */
                Stash stash;
                if((stash = stashes.findStash(commit->objectId())).isValid()) {
                    commit->setStash(true);

                    // Set the stash-base-of
                    GraphBuilderCommit* stashBaseCommit = map.value(stash.base().objectId());
                    if(stashBaseCommit == nullptr) {
                        throw GitException("Failed to find stash base");
                    }
                    stashBaseCommit->setStashBaseOf(commit->objectId());
                }
                else {
                    commit->setMerge(true);

                    // Find the merge-base and the merge-from
                    ObjectId::List parentIds = commit->parentObjectIds();
                    parentIds.append(commit->objectId());

                    Commit mergeBase = repo->objectDatabase()->findMergeBase(parentIds);
                    if(mergeBase.isValid() == false) {
                        throw GitException("Failed to find merge base commit");
                    }

                    parentIds.removeAll(commit->objectId());

                    commit->setMergeBase(mergeBase.objectId());
                    parentIds.removeAll(mergeBase.objectId());

                    if(parentIds.count() != 1) {
                        throw GitException("Too many merge parents left (bug)");
                    }

                    commit->setMergeFrom(parentIds.at(0));

                    // Set the merge-base-of
                    GraphBuilderCommit* mergeBaseCommit = map.value(commit->mergeBase());
                    if(mergeBaseCommit == nullptr) {
                        throw GitException("Failed to find merge base commit");
                    }
                    mergeBaseCommit->setMergedInto(commit->objectId());

                    GraphBuilderCommit* mergeFromCommit = map.value(commit->mergeFrom());
                    if(mergeFromCommit == nullptr) {
                        throw GitException("Failed to find merge from commit");
                    }
                    mergeFromCommit->setMergedInto(commit->objectId());
                }
           }
        }
    }
    catch(const GitException& e)
    {
        Log::logText(LVL_WARNING, e.message());
    }
}

void GraphBuilderCommit::PtrList::resolveBranchNames(GraphBuilder* graphBuilder)
{
    QString lastBranchName;
    for(GraphBuilderCommit* commit : *this) {
        // Resolve branch for this commit
        Branch commitBranch = graphBuilder->findBranchForReferencedObjectId(commit->objectId());
        if(commitBranch.isValid()) {
            commit->setHead(true);
            commit->setFriendlyBranchName(commitBranch.friendlyName(true));
            commit->setBranchName(commitBranch.friendlyName(false));
            commit->setRemote(commitBranch.isRemote());
        }
        else if(commit->childCommitsRef().count() > 0 || commit->isMerge()) {
            for(GraphBuilderCommit* child : commit->childCommitsRef()) {
                if(child->friendlyBranchName().isEmpty() == false) {
                    commit->copyBranchInformation(child);
                    break;
                }
            }
        }
        else if(commit->friendlyBranchName().isEmpty() == false) {
            // nothing... was already set. probably as merge base commit
        }
        else {
            // we remain on the same branch
            commit->setFriendlyBranchName(lastBranchName);
            Log::logText(LVL_WARNING, "Failed to resolve branch for commit");
        }

        // Look ahead... Set branch name of merge base
        if(commit->mergeBase().isValid()) {
            GraphBuilderCommit* mergeBaseCommit = graphBuilder->findCommit(commit->mergeBase());
            if(mergeBaseCommit != nullptr) {
                mergeBaseCommit->copyBranchInformation(commit);
            }
        }

        lastBranchName = commit->friendlyBranchName();
    }
}


