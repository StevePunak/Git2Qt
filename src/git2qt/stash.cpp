#include "stash.h"

#include <repository.h>

using namespace GIT;

Stash::Stash() :
    GitEntity(StashEntity, nullptr) {}

Stash::Stash(Repository* repo, const ObjectId& objectId, const QString& message, int index) :
    GitEntity(StashEntity, repo),
    _message(message), _index(index)
{
    _targetObject = repo->findCommit(objectId);
}

bool Stash::operator ==(const Stash& other) const
{
    return _targetObject == other._targetObject &&
            _message == other._message &&
            _index == other._index;
}

Commit Stash::base() const
{
    Commit commit;
    if(_targetObject.parents().count() > 0) {
        commit = _targetObject.parents().at(0);
    }
    return commit;
}

Commit Stash::index() const
{
    Commit commit;
    if(_targetObject.parents().count() > 1) {
        commit = _targetObject.parents().at(1);
    }
    return commit;
}

Commit Stash::untracked() const
{
    Commit commit;
    if(_targetObject.parents().count() > 2) {
        commit = _targetObject.parents().at(2);
    }
    return commit;
}
