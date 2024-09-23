/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of stashes. It is used by the repository
 * object to maintain the stashes.
 *
 * TODO: Should probably not be a public header.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STASHCOLLECTION_H
#define STASHCOLLECTION_H
#include <git2qt/gitentity.h>
#include <git2qt/remote.h>
#include <git2qt/stash.h>
#include <git2qt/stashapplyoptions.h>

namespace GIT {
class Repository;
class Signature;

class StashCollection : public GitEntity
{
public:
    StashCollection(Repository* repo);

    bool addStash(const Signature& stasher, const QString& message, StashModifiers options);
    bool popStash(const ObjectId& objectId, const StashApplyOptions& options = StashApplyOptions());
    bool deleteStash(const ObjectId& objectId);
    Stash findStash(const ObjectId& objectId) const { return _stashes.findStash(objectId); }

    Stash::List stashes() const { return _stashes; }

    virtual bool isNull() const override { return repository() == nullptr; }

private:
    void reload();

    static int stashCallback(size_t index, const char *message, const git_oid *stash_id, void *payload);

    Stash::List _stashes;
};

} // namespace GIT

#endif // STASHCOLLECTION_H
