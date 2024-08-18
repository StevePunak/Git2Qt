/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a single git commit.
 *
 * Stephen Punak, August 1, 2024
*/
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
    Commit();
    Commit(Repository* repo);
    Commit(Repository* repo, const ObjectId& objectId);
    virtual ~Commit();

    bool operator ==(const Commit& other) const;
    bool operator !=(const Commit& other) const { return !(*this == other); }

    static Commit lookup(Repository* repo, const ObjectId& objectId);

    Signature author() const { return _author; }
    Signature committer() const { return _committer; }
    QString message() const { return _message; }
    QString shortMessage() const { return _shortMessage; }
    QString encoding() const { return _encoding; }
    QDateTime timestamp() const { return _timestamp; }

    ObjectId treeId() const;
    Tree tree() const;

    QVariant toVariant() const { return QVariant::fromValue<Commit>(*this); }
    static Commit fromVariant(const QVariant& value) { return value.value<Commit>(); }

    bool isValid() const { return _timestamp.isValid(); }

    class List : public QList<Commit>
    {
    public:
        Commit findCommit(const ObjectId& objectId) const
        {
            Commit result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Commit& c) { return c.objectId() == objectId; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

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

Q_DECLARE_METATYPE(GIT::Commit)

#endif // COMMIT_H
