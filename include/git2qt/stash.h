/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a stash
 *
 * Stephen Punak, August 31, 2024
*/
#ifndef STASH_H
#define STASH_H
#include <git2qt/gitentity.h>
#include <git2qt/commit.h>

namespace GIT {

class Repository;
class GIT2QT_EXPORT Stash : public GitEntity
{
public:
    Stash();
    Stash(Repository* repo, const ObjectId& objectId, const QString& message, int index = 0);

    bool operator ==(const Stash& other) const;
    bool operator !=(const Stash& other) const { return !(*this == other); }

    Commit workTree() const { return _targetObject; }
    Commit base() const;
    Commit index() const;
    Commit untracked() const;
    QString message() const { return _message; }

    bool isValid() const { return isNull() == false; }
    virtual bool isNull() const override { return _targetObject.isValid() == false; }

    QVariant toVariant() const { return QVariant::fromValue<Stash>(*this); }
    static Stash fromVariant(const QVariant& value) { return value.value<Stash>(); }

    class List : public QList<Stash>
    {
    public:
        Stash findStash(const ObjectId& objectId) const
        {
            Stash result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Stash& a) { return a.workTree().objectId() == objectId; });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    Commit _targetObject;
    QString _message;
    int _index;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::Stash)

#endif // STASH_H
