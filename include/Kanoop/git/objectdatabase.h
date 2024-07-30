#ifndef OBJECTDATABASE_H
#define OBJECTDATABASE_H
#include <Kanoop/git/commit.h>
#include <Kanoop/git/gitentity.h>

namespace GIT {

class ObjectDatabase : public GitEntity
{
public:
    explicit ObjectDatabase(Repository* repo);

    Commit createCommit(const Signature& author, const Signature& committer, const QString& message, Tree* tree, const Commit::List& parents, bool prettifyMessage, const QChar& commentChar = QChar('#'));

    virtual bool isNull() const { return false; }
};

} // namespace GIT

#endif // OBJECTDATABASE_H
