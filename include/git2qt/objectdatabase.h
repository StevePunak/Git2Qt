/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is a port of the ObjectDatabase class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef OBJECTDATABASE_H
#define OBJECTDATABASE_H
#include <git2qt/commit.h>
#include <git2qt/gitentity.h>
#include <git2qt/trackingdetails.h>

namespace GIT {

class Blob;

class GIT2QT_EXPORT ObjectDatabase : public GitEntity
{
public:
    explicit ObjectDatabase(Repository* repo);

    Commit createCommit(const Signature& author, const Signature& committer, const QString& message, const Tree& tree, const Commit::List& parents, bool prettifyMessage, const QChar& commentChar = QChar('#'));
    Commit findMergeBase(const Commit& a, const Commit& b, MergeBaseFindingStrategy strategy = MergeBaseFindStandard) const;
    Commit findMergeBase(const Commit::List& commits, MergeBaseFindingStrategy strategy = MergeBaseFindStandard) const;
    Commit findMergeBase(const ObjectId::List& objectIds, MergeBaseFindingStrategy strategy = MergeBaseFindStandard) const;

    TrackingDetails calculateHistoryDivergence(const Commit& local, const Commit& upstream) const;

    QByteArray readBlobData(const Blob& blob);

    virtual bool isNull() const { return false; }

    ObjectDatabaseHandle createHandle() const;
};

} // namespace GIT

#endif // OBJECTDATABASE_H
