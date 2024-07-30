#ifndef COMMIT_H
#define COMMIT_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/signature.h>
#include <Kanoop/git/objectid.h>


namespace GIT {

class Tree;
class ObjectId;

class Repository;

class Commit
{
public:
    Commit(Repository* repo) :
        _repo(repo) {}
    virtual ~Commit();

    static Commit lookup(Repository* repo, const ObjectId& objectId);

    Signature author() const { return _author; }
    Signature committer() const { return _committer; }
    QString message() const { return _message; }
    QString shortMessage() const { return _shortMessage; }
    QString encoding() const { return _encoding; }
    QDateTime timestamp() const { return _timestamp; }
    ObjectId objectId() const { return _objectId; }

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
    static Commit createFromNative(Repository* repo, git_commit* commit);

    Signature _author;
    Signature _committer;
    QString _message;
    QString _shortMessage;
    QString _encoding;
    QDateTime _timestamp;
    ObjectId _objectId;
    Commit::List _parents;

    Repository* _repo = nullptr;
};

} // namespace GIT

#endif // COMMIT_H
