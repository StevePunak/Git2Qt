#include "objectdatabase.h"

#include <repository.h>
#include <tree.h>

#include <Kanoop/commonexception.h>

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

        const git_commit* parentCommits[parents.count()] = { 0 };
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

        result = repository()->lookupCommit(commitOid);
    }
    catch(const CommonException&)
    {
    }
    return result;
}
