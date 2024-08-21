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
#include <gitexception.h>
#include <tree.h>
#include <diffoptions.h>
#include "log.h"

using namespace GIT;

TreeChanges Diff::compare(DiffModifiers diffModifiers, const QStringList& paths, const CompareOptions& compareOptions)
{
    TreeChanges result;

    IndexHandle indexHandle = repository()->index()->createHandle();
    try
    {
        git_diff* diff = nullptr;
        git_diff_options options = GIT_DIFF_OPTIONS_INIT;
        throwIfTrue(indexHandle.isNull());
        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), &options));
        git_diff_free(diff);
        diff = nullptr;

        // Build options (
        DiffOptions diffOptions = buildDiffOptions(diffModifiers, paths, compareOptions);
        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), diffOptions.toNative()));
        DiffHandle diffHandle(diff);
        result = buildTreeChanges(diffHandle);
        diffHandle.dispose();
    }
    catch(const GitException&)
    {
    }
    indexHandle.dispose();
    return result;
}

TreeChanges Diff::compare(const Tree& fromTree, const Tree& toTree)
{
    return compare(fromTree, toTree, DiffModifier::DiffModNone, CompareOptions());
}

TreeChanges Diff::compare(const Tree& fromTree, const Tree& toTree, DiffModifiers diffOptions, const CompareOptions& compareOptions)
{
    TreeChanges result;

    TreeHandle fromHandle = fromTree.createTreeHandle();
    TreeHandle toHandle = toTree.createTreeHandle();

    try
    {
        git_diff* diff = nullptr;

        DiffOptions options = buildDiffOptions(diffOptions, QStringList(), compareOptions);

        throwIfTrue(fromHandle.isNull());
        throwIfTrue(toHandle.isNull());
        throwOnError(git_diff_tree_to_tree(&diff, repository()->handle().value(), fromHandle.value(), toHandle.value(), options.toNative()));

        DiffHandle handle(diff);
        result = buildTreeChanges(handle);
        handle.dispose();
    }
    catch(const GitException&)
    {
    }

    fromHandle.dispose();
    toHandle.dispose();
    return result;
}

TreeChanges Diff::compare(const Tree& oldTree, DiffTargets diffTargets, const QStringList& paths, const CompareOptions& compareOptions)
{
    TreeChanges changes;
    ObjectId oldTreeId = oldTree.objectId();
    DiffModifiers diffOptions = diffTargets == DiffTargetWorkingDirectory ? DiffModIncludeUntracked : DiffModNone;
    DiffHandle handle = buildDiffList(oldTreeId, diffOptions, paths, compareOptions);
    if(handle.isNull() == false) {
        changes = buildTreeChanges(handle);
        handle.dispose();
    }
    return changes;
}

DiffDelta::List Diff::listDiffs(const CompareOptions& compareOptions, DiffModifiers diffFlags)
{
    DiffDelta::List collection;
    git_diff* diff = nullptr;

    IndexHandle indexHandle = repository()->index()->createHandle();
    try
    {
        DiffOptions options = buildDiffOptions(diffFlags, QStringList() << "*", compareOptions);

        throwIfTrue(indexHandle.isNull());

        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), options.toNative()));
        int count = git_diff_num_deltas(diff);
        if(count > 0) {
            git_diff_foreach(diff, fileCallback, binaryCallback, hunkCallback, lineCallback, &collection);
        }
    }
    catch(const GitException&)
    {
    }

    if(diff != nullptr) {
        git_diff_free(diff);
    }
    indexHandle.dispose();
    return collection;
}

DiffOptions Diff::buildDiffOptions(DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions)
{
    DiffOptions options;

    options.setFlags(DiffOptionIncludeTypeChange);
    options.setContextLines(compareOptions.contextLines());
    options.setInterhunkLines(compareOptions.interhunkLines());

    if(diffOptions & DiffModIncludeUntracked) {
        options.setFlags(DiffOptionIncludeUntracked | DiffOptionRecurseUntrackedDirs | DiffOptionShowUntrackedContent);
    }
    if(diffOptions & DiffModIncludeIgnored) {
        options.setFlags(DiffOptionIncludeIgnored | DiffOptionRecurseIgnoredDirs);
    }
    if(((diffOptions & DiffModIncludeUnmodified) || compareOptions.includeUnmodified()) ||
        compareOptions.similarity().renameDetectionMode() == SimilarityOptions::RenameDetectionCopiesHarder ||
        compareOptions.similarity().renameDetectionMode() == SimilarityOptions::RenameDetectionExact) {
        options.setFlags(DiffOptionIncludeUnmodified);
    }

    if(compareOptions.algorithm() == Patience) {
        options.setFlags(DiffOptionPatience);
    }
    else if(compareOptions.algorithm() == Minimal) {
        options.setFlags(DiffOptionMinimal);
    }

    if(diffOptions & DiffModDisablePathspecMatch) {
        options.setFlags(DiffOptionDisablePathspecMatch);
    }

    if(compareOptions.indentHeurisitc()) {
        options.setFlags(DiffOptionIndentHeuristic);
    }

    options.setPaths(paths);

    return options;
}

DiffHandle Diff::buildDiffList(const ObjectId& oldTreeId, DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions)
{
    git_diff* diff = nullptr;
    TreeHandle treeHandle;
    DiffHandle result;
    try
    {
        DiffOptions options = buildDiffOptions(diffOptions, paths, compareOptions);
        Tree oldTree = repository()->lookupTree(oldTreeId);
        throwIfTrue(oldTree.isNull());
        treeHandle = oldTree.createTreeHandle();
        const git_diff_options* opts = options.toNative();
        throwOnError(git_diff_tree_to_index(&diff, repository()->handle().value(), treeHandle.value(), repository()->index()->createHandle().value(), opts));
        result = DiffHandle(diff);
    }
    catch(const GitException& e)
    {
    }

    treeHandle.dispose();

    return result;
}

void Diff::detectRenames(const DiffHandle& handle, const CompareOptions& compareOptions)
{
    SimilarityOptions similarityOptions = compareOptions.similarity();
    if(similarityOptions.renameDetectionMode() == SimilarityOptions::RenameDetectionDefault) {
        git_diff_find_similar(handle.value(), nullptr);
        return;
    }

    if(similarityOptions.renameDetectionMode() == SimilarityOptions::RenameDetectionNone) {
        return;
    }

    git_diff_find_options opts = similarityOptions.toNativeDiffFindOptions();
    if(compareOptions.includeUnmodified()) {
        opts.flags |= DiffFindRemoveUnmodified;
    }

    git_diff_find_similar(handle.value(), &opts);
}

TreeChanges Diff::buildTreeChanges(const DiffHandle& handle)
{
    TreeChanges changes;
    int count = git_diff_num_deltas(handle.value());
    for(int i = 0;i < count;i++) {
        const git_diff_delta* delta = git_diff_get_delta(handle.value(), i);
        TreeChangeEntry change(delta);
        changes.append(change);
    }
    return changes;
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
        Log::logText(LVL_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
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
        Log::logText(LVL_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
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
            Log::logText(LVL_DEBUG, QString("FAILED TO FIND MATCHING HUNK!!!!"));
        }
    }
    else {
        Log::logText(LVL_DEBUG, QString("FAILED TO FIND MATCHING DELTA!!!!"));
    }
    return 0;
}
