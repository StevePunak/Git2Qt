/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents one git_reflog_entry
 *
 * Stephen Punak, August 21, 2024
*/
#ifndef REFLOGENTRY_H
#define REFLOGENTRY_H
#include <git2qt/gitentity.h>
#include <git2qt/objectid.h>
#include <git2qt/signature.h>

namespace GIT {

class GIT2QT_EXPORT ReflogEntry
{
public:
    ReflogEntry(const git_reflog_entry* entry);

    ObjectId from() const { return _from; }
    ObjectId to() const { return _to; }
    Signature committer() const { return _committer; }
    QString message() const { return _message; }

    class List : public QList<ReflogEntry>
    {
    };

private:
    ObjectId _from;
    ObjectId _to;
    Signature _committer;
    QString _message;
};

} // namespace GIT

#endif // REFLOGENTRY_H
