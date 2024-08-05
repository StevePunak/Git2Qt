#ifndef COMMIT_H
#define COMMIT_H
#include <Kanoop/git/gitobject.h>
#include <Kanoop/git/signature.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/handle.h>

namespace GIT {

class Tree;
class Repository;
class Commit : public GitObject
{
public:
    Commit(Repository* repo);
    Commit(Repository* repo, const ObjectId& objectId);
    virtual ~Commit();

    static Commit lookup(Repository* repo, const ObjectId& objectId);

    Signature author() const { return _author; }
    Signature committer() const { return _committer; }
    QString message() const { return _message; }
    QString shortMessage() const { return _shortMessage; }
    QString encoding() const { return _encoding; }
    QDateTime timestamp() const { return _timestamp; }

    ObjectId treeId() const;

    bool isValid() const { return _timestamp.isValid(); }

    class List : public QList<Commit>
    {
    public:
        ObjectId::List objectIds() const
        {
            ObjectId::List result;
            for(const Commit& commit : *this) {
                result.append(commit.objectId());
            }
            return result;
        }
    };

    Commit::List parents() const;

private:
    CommitHandle createHandle() const;

    static Commit createFromNative(Repository* repo, git_commit* commit);

    Signature _author;
    Signature _committer;
    QString _message;
    QString _shortMessage;
    QString _encoding;
    QDateTime _timestamp;
    Commit::List _parents;
};

} // namespace GIT

#endif // COMMIT_H
