#include "ancestorseeker.h"

#include <QElapsedTimer>
#include <gitexception.h>
#include <repository.h>
#include <utility.h>

using namespace GIT;

MergeBaseSeeker::MergeBaseSeeker(GraphBuilderCommit *mergeCommit, const GraphBuilderCommit::Map& commitIndex) :
    _mergeCommit(mergeCommit), _commitIndex(commitIndex), _birthCommit(nullptr)
{
    resolve2();
}

void MergeBaseSeeker::resolve1()
{
}

void MergeBaseSeeker::resolve2()
{
#undef PERF_TEST
#ifdef PERF_TEST
    static TimeSpan longest;
    QElapsedTimer timer;
    timer.start();
#endif
    // we will find a common ancestor to all parents
    SeekerSet seekers;
    for(GraphBuilderCommit* parent : _mergeCommit->parentCommitsRef()) {
        seekers.append(new AncestorSeeker(parent));
    }

    _birthCommit = nullptr;
    while(_birthCommit == nullptr) {
        for(AncestorSeeker* seeker : seekers) {
            seeker->produceAncestors(10);
        }

        AncestorSeeker* first = seekers.at(0);
        ObjectId::Set& firstSeekerIds = first->ancestors();

        for(const ObjectId& firstId : firstSeekerIds) {
            int foundCount = 1;
            for(int i = 1;i < seekers.count();i++) {
                AncestorSeeker* other = seekers.at(i);
                ObjectId::Set& otherSeekerIds = other->ancestors();
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

#ifdef PERF_TEST
    TimeSpan elapsed = TimeSpan::fromNanoseconds(timer.nsecsElapsed());
    if(elapsed > longest) {
        longest = elapsed;
        Log::logText(LVL_DEBUG, QString("Seeker for %1 took %2").arg(_mergeCommit->objectId().toString()).arg(longest.toString(true)));
    }
#endif
    qDeleteAll(seekers);
}


AncestorSeeker::AncestorSeeker(GraphBuilderCommit *commit) :
    _commit(commit), _oldestAncestor(commit->timestamp())
{
    _ancestors.insert(commit->objectId());
    for(GraphBuilderCommit* parent : commit->parentCommitsRef()) {
        _tips.insert(parent);
    }
}

void AncestorSeeker::produceAncestors(int count)
{
    // Log::logText(LVL_DEBUG, QString("Produce ancestors for %1 (tips: %2  ancestors: %3  oldest: %4)")
    //              .arg(_commit->objectId().toString())
    //              .arg(_tips.count())
    //              .arg(_ancestors.count())
    //              .arg(Utility::toString(_oldestAncestor)));
    int resultCount = 0;
    while((resultCount < count) && _tips.count() > 0) {
        GraphBuilderCommit::Set tips = _tips;
        for(GraphBuilderCommit* tip : tips) {
            resultCount++;
            _ancestors.insert(tip->objectId());
            if(tip->timestamp() < _oldestAncestor) {
                _oldestAncestor = tip->timestamp();
            }
            _tips.remove(tip);
            for(GraphBuilderCommit* parent : tip->parentCommitsRef()) {
                _tips.insert(parent);
            }
        }
    }
}


// ---------------------------------- SeekerSet ----------------------------------

void SeekerSet::produceAncestors()
{
    static const int QUANTA = 10;
    for(AncestorSeeker* seeker : *this) {
        seeker->produceAncestors(QUANTA);
    }
}
