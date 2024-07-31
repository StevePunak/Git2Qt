#include "diff.h"
#include <compareoptions.h>
#include <git2.h>
#include <repository.h>
#include <stringarray.h>
#include <treechanges.h>
#include <index.h>
#include <Kanoop/commonexception.h>

using namespace GIT;

TreeChanges Diff::compare(DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions)
{
    TreeChanges result;
    git_diff* diff = nullptr;

    try
    {
        git_diff_options options = GIT_DIFF_OPTIONS_INIT;
        IndexHandle indexHandle = repository()->index()->handle();
        throwIfTrue(indexHandle.isNull());
        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), &options));
        git_diff_free(diff);
        diff = nullptr;

        // Build options (
        options.flags |= GIT_DIFF_INCLUDE_TYPECHANGE;
        options.context_lines = compareOptions.contextLines();
        options.interhunk_lines = compareOptions.interhunkLines();
        if(diffOptions & IncludeUntracked) {
            options.flags |= (GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS | GIT_DIFF_SHOW_UNTRACKED_CONTENT);
        }
        if(diffOptions & IncludeIgnored) {
            options.flags |= (GIT_DIFF_INCLUDE_IGNORED | GIT_DIFF_RECURSE_IGNORED_DIRS);
        }
        if((diffOptions & IncludeUnmodified || compareOptions.includeUnmodified()) ||
            compareOptions.similarity().renameDetectionMode() == SimilarityOptions::CopiesHarder || compareOptions.similarity().renameDetectionMode() == SimilarityOptions::Exact) {
            options.flags |= GIT_DIFF_INCLUDE_UNMODIFIED;
        }

        if(compareOptions.algorithm() == Patience) {
            options.flags |= GIT_DIFF_PATIENCE;
        }
        else if (compareOptions.algorithm() == Minimal) {
            options.flags |= GIT_DIFF_MINIMAL;
        }

        if (diffOptions & DisablePathspecMatch) {
            options.flags |= GIT_DIFF_DISABLE_PATHSPEC_MATCH;
        }

        if (compareOptions.identHeurisitc()) {
            options.flags |= GIT_DIFF_INDENT_HEURISTIC;
        }
#if 0
        if (matchedPathsAggregator != null) {
            options.notify_cb = nullptr; // TODO set up callback
        }
#endif
        StringArray pathArray(paths);
        char* pathPtrs[paths.count()];
        for(int i = 0;i < paths.count();i++) {
            pathPtrs[i] = pathArray.at(i);
        }
        options.pathspec.strings = pathPtrs;
        options.pathspec.count = paths.count();

        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), &options));
        int count = git_diff_num_deltas(diff);
        for(int i = 0;i < count;i++) {
            const git_diff_delta* delta = git_diff_get_delta(diff, i);
            TreeEntryChanges change(delta);
            result.append(change);
        }

    }
    catch(const CommonException&)
    {
    }

    if(diff != nullptr) {
        git_diff_free(diff);
    }
    return result;
}
