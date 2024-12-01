#include "objectdatabase.h"

#include <gitexception.h>
#include <repository.h>
#include <tree.h>

using namespace GIT;

ObjectDatabase::ObjectDatabase(Repository* repo) :
    GitEntity(ObjectDatabaseEntity, repo)
{
}

Commit ObjectDatabase::createCommit(const Signature& author, const Signature& committer,
                                    const QString& message,
                                    const Tree& tree,
                                    const Commit::List& parents,
                                    bool prettifyMessage,
                                    const QChar& commentChar)
{
    Commit result(repository());
    try
    {
        QString msg = message;
        if(prettifyMessage) {
            git_buf buf = GIT_BUF_INIT;
            if(git_message_prettify(&buf, message.toUtf8().constData(), true, commentChar.toLatin1()) == 0) {
                msg = buf.ptr;
            }
        }

        const git_commit* parentCommits[parents.count()];
        for(int i = 0;i < parents.count();i++) {
            git_commit* commit = nullptr;
            if(git_commit_lookup(&commit, repository()->handle().value(), parents.at(i).objectId().toNative()) == 0) {
                parentCommits[i] = commit;
            }
        }

        Signature authorSig = author;
        Signature committerSig = committer;
        git_oid commitOid;
        TreeHandle treeHandle = tree.createTreeHandle();
        git_repository* repoHandle = repository()->handle().value();
        git_tree* tree = treeHandle.value();

        throwOnError(git_commit_create(&commitOid,
                                       repoHandle,
                                       nullptr,
                                       authorSig.toNative(),
                                       committerSig.toNative(),
                                       "utf8",
                                       msg.toUtf8().constData(),
                                       tree,
                                       parents.count(),
                                       parentCommits));
        treeHandle.dispose();

        result = repository()->findCommit(commitOid);
    }
    catch(const GitException&)
    {
    }
    return result;
}

Commit ObjectDatabase::findMergeBase(const Commit& a, const Commit& b, MergeBaseFindingStrategy strategy) const
{
    Commit::List commits;
    commits.append(a);
    commits.append(b);
    return findMergeBase(commits, strategy);
}

Commit ObjectDatabase::findMergeBase(const Commit::List& commits, MergeBaseFindingStrategy strategy) const
{
    return findMergeBase(commits.objectIds(), strategy);
}

Commit ObjectDatabase::findMergeBase(const ObjectId::List& objectIds, MergeBaseFindingStrategy strategy) const
{
    Commit baseCommit;
    try
    {
        if(objectIds.count() < 2) {
            throw GitException("Find merge base must take at least two commits");
        }

        git_oid oids[objectIds.count()];
        for(int i = 0;i < objectIds.count();i++) {
            oids[i] = *objectIds.at(i).toNative();
        }

        git_oid base;
        switch(strategy) {
        case MergeBaseFindStandard:
            throwOnError(git_merge_base_many(&base, repository()->handle().value(), objectIds.count(), oids));
            break;
        case MergeBaseFindOctopus:
            throwOnError(git_merge_base_octopus(&base, repository()->handle().value(), objectIds.count(), oids));
            break;
        default:
            throw GitException("Invalid parameter");
        }

        baseCommit = Commit::lookup(repository(), base);
    }
    catch(const GitException&)
    {
    }
    return baseCommit;
}

TrackingDetails ObjectDatabase::calculateHistoryDivergence(const Commit& local, const Commit& upstream) const
{
    TrackingDetails divergence;

    try
    {
        size_t ahead, behind;
        throwOnError(git_graph_ahead_behind(&ahead, &behind, repository()->handle().value(), local.objectId().toNative(), upstream.objectId().toNative()));

        divergence = TrackingDetails(local, upstream, ahead, behind);
    }
    catch(const GitException&)
    {
    }


    return divergence;
}

QByteArray ObjectDatabase::readBlobData(const Blob& blob)
{
    QByteArray result;

    try
    {
        ObjectDatabaseHandle handle = createHandle();
        throwIfTrue(handle.isNull());

        // locate the object
        git_odb_object* obj = nullptr;
        throwOnError(git_odb_read(&obj, handle.value(), blob.objectId().toNative()));

        // read len and type
        size_t len = git_odb_object_size(obj);
        git_object_t type = git_odb_object_type(obj);
        throwIfTrue(type != GIT_OBJECT_BLOB);

        const void* data = git_odb_object_data(obj);
        result = QByteArray((const char*)data, len);
    }
    catch(const GitException&)
    {
    }
    return result;
}

ObjectDatabaseHandle ObjectDatabase::createHandle() const
{
    ObjectDatabaseHandle handle;
    git_odb* odb = nullptr;
    if(git_repository_odb(&odb, repository()->handle().value()) == 0) {
        handle = ObjectDatabaseHandle(odb);
    }
    return handle;
}
