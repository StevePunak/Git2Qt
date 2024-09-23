#include "ancestorseeker.h"

#include <gitexception.h>
#include <repository.h>

using namespace GIT;

MergeBaseSeeker::MergeBaseSeeker(GraphBuilderCommit *mergeCommit, const GraphBuilderCommit::Map& commitIndex) :
    _mergeCommit(mergeCommit), _commitIndex(commitIndex), _birthCommit(nullptr)
{
    resolve2();
}

void MergeBaseSeeker::resolve1()
{
#if 0
    MergeTipIndex tips;
    for(GraphBuilderCommit* parent : _mergeCommit->parentCommitsRef()) {
        tips.insert(parent->objectId(), new MergeTip(parent));
    }

    _birthCommit = nullptr;
    while(_birthCommit == nullptr) {
        for(MergeTip* mergeTip : tips) {

        }
    }




    Repository* repo = _mergeCommit->repository();

    git_revwalk *walker = nullptr;

    Commit::List commits;
    try
    {
        git_revwalk_new(&walker, repo->handle().value());
        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
        git_revwalk_push(walker, _mergeCommit->objectId().toNative());

        git_oid oid;
        while(git_revwalk_next(&oid, walker) == 0) {
            Commit commit = Commit::lookup(this, ObjectId(oid));
            if(commit.isValid() == false) {
                throw GitException("Failed to lookup the next object");
            }
            commits.append(commit);
        }
    }
    catch(const GitException&)
    {

    }

    if(walker != nullptr) {
        git_revwalk_free(walker);
    }
#endif
}

void MergeBaseSeeker::resolve2()
{
    // we will find a common ancestor to all parents
    QList<AncestorSeeker*> seekers;
    for(GraphBuilderCommit* parent : _mergeCommit->parentCommitsRef()) {
        seekers.append(new AncestorSeeker(parent));
    }

    _birthCommit = nullptr;
    while(_birthCommit == nullptr) {
        for(AncestorSeeker* seeker : seekers) {
            seeker->produceAncestors(1);
        }

        AncestorSeeker* first = seekers.at(0);
        ObjectId::List firstSeekerIds = first->ancestors();

        for(const ObjectId& firstId : firstSeekerIds) {
            int foundCount = 1;
            for(int i = 1;i < seekers.count();i++) {
                AncestorSeeker* other = seekers.at(i);
                ObjectId::List otherSeekerIds = other->ancestors();
                if(otherSeekerIds.contains(firstId)) {
                    foundCount++;
                }
            }
            if(foundCount >= seekers.count()) {
                _birthCommit = _commitIndex.value(firstId);
                break;
            }
        }
    }

    qDeleteAll(seekers);
}


AncestorSeeker::AncestorSeeker(GraphBuilderCommit *commit) :
    _commit(commit)
{
    _ancestors.append(commit->objectId());
    for(GraphBuilderCommit* parent : commit->parentCommitsRef()) {
        _tips.append(parent);
    }
}

ObjectId::List AncestorSeeker::produceAncestors(int count)
{
    ObjectId::List result;
    while(result.count() < count && _tips.count() > 0) {
        GraphBuilderCommit::PtrList tips = _tips;
        for(GraphBuilderCommit* tip : tips) {
            result.append(tip->objectId());
            _ancestors.append(tip->objectId());
            _tips.removeAll(tip);
            for(GraphBuilderCommit* parent : tip->parentCommitsRef()) {
                _tips.append(parent);
            }
        }
    }
    return result;
}



