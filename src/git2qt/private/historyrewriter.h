#ifndef HISTORYREWRITER_H
#define HISTORYREWRITER_H
#include <git2qt/gitentity.h>
#include <git2qt/commit.h>

namespace GIT {

class RewriteHistoryOptions;

class HistoryRewriter : public GitEntity
{
public:
    HistoryRewriter(Repository* repo, const Commit::List& commits, RewriteHistoryOptions* options);

    bool execute();

    virtual bool isNull() const override { return repository() == nullptr; }

private:
    bool rewriteCommit(const Commit& commit, RewriteHistoryOptions* options);

    Commit::List _targetedCommits;
    RewriteHistoryOptions* _options = nullptr;

    QString _backupRefsNamespace;

    QMap<Commit, Commit> _objectMap;
};

} // namespace GIT

#endif // HISTORYREWRITER_H
