#include "diff.h"
#include <compareoptions.h>
#include <git2.h>
#include <repository.h>
#include <stringarray.h>
#include <treechanges.h>
#include <index.h>
#include <diffdelta.h>
#include <diffbinary.h>
#include <diffhunk.h>
#include <diffline.h>
#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

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
        buildDiffOptions(diffOptions, compareOptions, options);

        StringArray pathArray(paths);
        options.pathspec = *pathArray.native();

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

DiffDelta::List Diff::listDiffs(const CompareOptions& compareOptions, DiffModifiers diffFlags)
{
    DiffDelta::List collection;
    git_diff* diff = nullptr;

    try
    {
        git_diff_options options;
        buildDiffOptions(diffFlags, compareOptions, options);
        StringArray pathArray("*");
        options.pathspec = *pathArray.native();

        IndexHandle indexHandle = repository()->index()->handle();
        throwIfTrue(indexHandle.isNull());

        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), &options));
        int count = git_diff_num_deltas(diff);
        if(count > 0) {
            git_diff_foreach(diff, fileCallback, binaryCallback, hunkCallback, lineCallback, &collection);
        }
    }
    catch(const CommonException&)
    {
    }

    if(diff != nullptr) {
        git_diff_free(diff);
    }
    return collection;
}

void Diff::buildDiffOptions(DiffModifiers diffOptions, const CompareOptions& compareOptions, git_diff_options& options)
{
    options = GIT_DIFF_OPTIONS_INIT;

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
}

int Diff::fileCallback(const git_diff_delta* delta, float progress, void* payload)
{
    Q_UNUSED(progress)
    DiffDelta::List* collection = static_cast<DiffDelta::List*>(payload);
    collection->append(DiffDelta(delta));
    return 0;
}

int Diff::binaryCallback(const git_diff_delta* d, const git_diff_binary* binary, void* payload)
{
    DiffDelta::List* collection = static_cast<DiffDelta::List*>(payload);
    DiffDelta* delta = collection->getDiffDeltaPtr(DiffDelta(d));
    if(delta != nullptr) {
        delta->appendBinary(DiffBinary(binary));
    }
    else {
        KLog::sysLogText(KLOG_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
    }
    return 0;
}

int Diff::hunkCallback(const git_diff_delta* d, const git_diff_hunk* h, void* payload)
{
    DiffDelta::List* collection = static_cast<DiffDelta::List*>(payload);
    DiffDelta* xdelta = collection->getDiffDeltaPtr(DiffDelta(d));
    if(xdelta != nullptr) {
        xdelta->appendHunk(DiffHunk(h));
    }
    else {
        KLog::sysLogText(KLOG_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
    }
    return 0;
}

int Diff::lineCallback(const git_diff_delta* d, const git_diff_hunk* h, const git_diff_line* l, void* payload)
{
    DiffDelta::List* collection = static_cast<DiffDelta::List*>(payload);
    DiffDelta* delta = collection->getDiffDeltaPtr(DiffDelta(d));
    if(delta != nullptr) {
        DiffHunk* hunk = delta->hunksRef().getDiffHunkPtr(DiffHunk(h));
        if(hunk != nullptr) {
            hunk->appendLine(DiffLine(l));
        }
        else {
            KLog::sysLogText(KLOG_DEBUG, QString("FAILED TO FIND MATCHING HUNK!!!!"));
        }
    }
    else {
        KLog::sysLogText(KLOG_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
    }
    return 0;
}
