#ifndef COMMITLOG_H
#define COMMITLOG_H
#include <git2qt/gitentity.h>
#include <git2qt/commitfilter.h>
#include <git2qt/commit.h>
#include <git2qt/handle.h>

namespace GIT {

class Commit;
class Repository;
class GIT2QT_EXPORT CommitLog : public GitEntity
{
public:
    CommitLog(Repository* repo, const CommitFilter& filter);

    Commit::List performLookup();
    void findReachableFrom(const Commit& commit);

    virtual bool isNull() const override { return repository() != nullptr; }

private:
    RevWalkHandle createWalkerHandle() const;

    CommitFilter _filter;
};

} // namespace GIT

#endif // COMMITLOG_H
