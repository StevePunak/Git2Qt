#include "stashcollection.h"

#include <gitexception.h>
#include <repository.h>

using namespace GIT;

StashCollection::StashCollection(Repository* repo) :
    GitEntity(StashCollectionEntity, repo)
{
    reload();
}

bool StashCollection::addStash(const Signature& stasher, const QString& message, StashModifiers options)
{
    bool result = false;
    try
    {
        QString msg = message;
        if(msg.isEmpty() == false) {
            git_buf buf = GIT_BUF_INIT;
            throwOnError(git_message_prettify(&buf, msg.toUtf8().constData(), true, '#'));
            Signature sig = stasher;
            git_oid oid;
            throwOnError(git_stash_save(&oid, repository()->handle().value(), sig.toNative(), msg.toUtf8().constData(), options));
            ObjectId target(oid);
            if(target.isValid()) {
                Stash stash(repository(), target, msg, 0);
                _stashes.append(stash);
                reload();
                result = true;
            }
            git_buf_free(&buf);
        }
    }
    catch(const GitException&)
    {
    }
    return result;
}

bool StashCollection::popStash(const ObjectId& objectId, const StashApplyOptions& options)
{
    bool result = false;
    try
    {
        Stash stash = findStash(objectId);
        throwIfFalse(stash.isValid());

        int index = _stashes.indexOf(stash);
        throwIfTrue(index < 0, "Invalid stash");

        StashApplyOptions opts = options;
        throwOnError(git_stash_pop(repository()->handle().value(), index, opts.toNative()));

        reload();

        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

bool StashCollection::deleteStash(const ObjectId& objectId)
{
    bool result = false;
    try
    {
        Stash stash = findStash(objectId);
        throwIfFalse(stash.isValid());

        int index = _stashes.indexOf(stash);
        throwIfTrue(index < 0, "Invalid stash");

        throwOnError(git_stash_drop(repository()->handle().value(), index));

        reload();

        result = true;
    }
    catch(const GitException&)
    {
    }
    return result;
}

void StashCollection::reload()
{
    try
    {
        _stashes.clear();
        throwOnError(git_stash_foreach(repository()->handle().value(), stashCallback, this));
    }
    catch(const GitException&)
    {
    }
}

int StashCollection::stashCallback(size_t index, const char* message, const git_oid* stash_id, void* payload)
{
    StashCollection* collection = static_cast<StashCollection*>(payload);
    Stash stash(collection->repository(), ObjectId(stash_id), message, index);
    collection->_stashes.append(stash);
    return 0;
}
