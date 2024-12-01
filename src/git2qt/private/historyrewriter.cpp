#include "historyrewriter.h"

#include <commitlog.h>
#include <commitrewriteinfo.h>
#include <gitexception.h>
#include <reference.h>
#include <repository.h>
#include <rewritehistoryoptions.h>
#include <tree.h>

using namespace GIT;

HistoryRewriter::HistoryRewriter(Repository* repo, const Commit::List& commits, RewriteHistoryOptions* options) :
    GitEntity(GitEntityType::HistoryRewriterEntity, repo),
    _targetedCommits(commits), _options(options)
{
    _backupRefsNamespace = options->backupRefsNamespace();
    if(_backupRefsNamespace.endsWith('/') == false) {
        _backupRefsNamespace += '/';
    }
}

bool HistoryRewriter::execute()
{
    bool success = false;
    try
    {
        ReferenceList refsToRewrite = repository()->findReferencesReachableFrom(_targetedCommits);

        CommitFilter commitFilter;
        commitFilter.setIncludeReachableFrom(refsToRewrite.objectIds());
        commitFilter.setSortBy(SortStrategyTopological | SortStrategyReverse);

        CommitLog commitLog(repository(), commitFilter);
        Commit::List commits = commitLog.performLookup();
        for(const Commit& commit : commits) {
            rewriteCommit(commit, _options);
        }

        // Ordering matters. In the case of `A -> B -> commit`, we need to make sure B is rewritten
        // before A.
        std::sort(refsToRewrite.begin(), refsToRewrite.end(), [](const Reference& a, const Reference& b)
        {
            return a.depth() < b.depth();
        });



    }
    catch(const GitException&) {

    }
    return success;
}

bool HistoryRewriter::rewriteCommit(const Commit& commit, RewriteHistoryOptions* options)
{
    Q_UNUSED(commit) Q_UNUSED(options)
    CommitRewriteInfo newHeader = CommitRewriteInfo::fromCommit(commit);
    Tree newTree = commit.tree();

    Commit::List newParents = commit.parents();
    if(_targetedCommits.contains(commit)) {

    }

    Commit::List mappedNewParents;
    for(const Commit& newParent : newParents) {
        if(_objectMap.contains(newParent)) {
            mappedNewParents.append(_objectMap[newParent]);
        }
        else {
            mappedNewParents.append(newParent);
        }
    }
    Commit newCommit = repository()->objectDatabase()->createCommit(newHeader.author(), newHeader.committer(), newHeader.message(), newTree, mappedNewParents, options->prettifyMessages());
    _objectMap[commit] = newCommit;

    return false;
}
