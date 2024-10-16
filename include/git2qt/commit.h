/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a single git commit.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef COMMIT_H
#define COMMIT_H
#include <git2qt/gitobject.h>
#include <git2qt/signature.h>
#include <git2qt/objectid.h>
#include <git2qt/handle.h>

namespace GIT {

class Tree;
class Repository;
class GIT2QT_EXPORT Commit : public GitObject
{
public:
    Commit();
    Commit(Repository* repo);
    Commit(Repository* repo, const ObjectId& objectId);

protected:
    Commit(GitEntityType entityType, Repository* repo, const ObjectId& objectId);

public:
    virtual ~Commit();

    bool operator ==(const Commit& other) const;
    bool operator !=(const Commit& other) const { return !(*this == other); }
    bool operator <(const Commit& other) const { return objectId() < other.objectId(); }
    bool operator >(const Commit& other) const { return objectId() > other.objectId(); }

    static Commit lookup(Repository* repo, const ObjectId& objectId);

    Signature author() const { return _author; }
    void setAuthor(const Signature& value) { _author = value; }

    Signature committer() const { return _committer; }
    void setCommitter(const Signature& value) { _committer = value; }

    QString message() const { return _message; }
    void setMessage(const QString& value) { _message = value; }

    QString shortMessage() const { return _shortMessage; }
    void setShortMessage(const QString& value) { _shortMessage = value; }

    QString encoding() const { return _encoding; }
    void setEncoding(const QString& value) { _encoding = value; }

    QDateTime timestamp() const { return _timestamp; }
    void setTimestamp(const QDateTime& value) { _timestamp = value; }


    ObjectId treeId() const;
    Tree tree() const;

    QVariant toVariant() const { return QVariant::fromValue<Commit>(*this); }
    static Commit fromVariant(const QVariant& value) { return value.value<Commit>(); }

    CommitHandle createHandle() const;

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

        int indexOfObjectId(const ObjectId& objectId) const
        {
            int result = -1;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Commit& commit) { return commit.objectId() == objectId; });
            if(it != constEnd()) {
                result = std::distance(constBegin(), it);
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
    bool isReachableFrom(const Commit& other) const;
    bool isReachableFromAny(const Commit::List& other) const;

private:
    void resolve();

    static Commit createFromNative(Repository* repo, git_commit* commit);

    Signature _author;
    Signature _committer;
    QString _message;
    QString _shortMessage;
    QString _encoding;
    QDateTime _timestamp;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::Commit)

#endif // COMMIT_H
