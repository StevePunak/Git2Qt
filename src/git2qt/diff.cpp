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

        // Build options
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

TreeChanges Diff::compare(const Commit& from, const Commit& to, const CompareOptions& compareOptions, DiffModifiers diffFlags)
{
    TreeChanges result;

    try
    {
        DiffHandle diffHandle = buildDiffList(from, to, compareOptions, diffFlags);
        throwIfTrue(diffHandle.isNull());
        result = buildTreeChanges(diffHandle);
        diffHandle.dispose();
    }
    catch(const GitException&)
    {
    }

    return result;
}

TreeChanges Diff::compare_DEP(const Tree& fromTree, const Tree& toTree)
{
    return compare(fromTree, toTree, DiffModifier::DiffModNone, CompareOptions());
}

TreeChanges Diff::compare(const Tree& fromTree, const Tree& toTree, DiffModifiers diffOptions, const CompareOptions& compareOptions)
{
    TreeChanges result;

    TreeHandle fromHandle = fromTree.createTreeHandle();
    TreeHandle toHandle = toTree.createTreeHandle();
    DiffHandle diffHandle;

    try
    {
        git_diff* diff = nullptr;

        DiffOptions options = buildDiffOptions(diffOptions, QStringList(), compareOptions);

        throwIfTrue(fromHandle.isNull());
        throwIfTrue(toHandle.isNull());
        throwOnError(git_diff_tree_to_tree(&diff, repository()->handle().value(), fromHandle.value(), toHandle.value(), options.toNative()));

        diffHandle = DiffHandle(diff);
        result = buildTreeChanges(diffHandle);
    }
    catch(const GitException&)
    {
    }

    fromHandle.dispose();
    toHandle.dispose();
    diffHandle.dispose();
    return result;
}

TreeChanges Diff::compare(const Tree& oldTree, DiffTargets diffTargets, const QStringList& paths, const CompareOptions& compareOptions)
{
    TreeChanges changes;
    ObjectId oldTreeId = oldTree.objectId();
    DiffModifiers diffOptions = diffTargets == DiffTargetWorkingDirectory ? DiffModIncludeUntracked : DiffModNone;
    DiffHandle diffHandle = buildDiffList(oldTreeId, diffOptions, paths, compareOptions);
    if(diffHandle.isNull() == false) {
        changes = buildTreeChanges(diffHandle);
        diffHandle.dispose();
    }
    return changes;
}

DiffDelta::List Diff::diffIndexToWorkDir(const QString& path, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    QStringList paths;
    paths.append(path);
    return diffIndexToWorkDir(paths, includeUntracked, compareOptions, diffFlags);
}

DiffDelta::List Diff::diffIndexToWorkDir(const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    DiffDelta::List collection;
    git_diff* diff = nullptr;

    IndexHandle indexHandle = repository()->index()->createHandle();
    DiffHandle diffHandle;
    try
    {
        if(includeUntracked) {
            diffFlags |= DiffModIncludeUntracked;
        }

        DiffOptions options = buildDiffOptions(diffFlags, paths, compareOptions);

        throwIfTrue(indexHandle.isNull());

        throwOnError(git_diff_index_to_workdir(&diff, repository()->handle().value(), indexHandle.value(), options.toNative()));

        diffHandle = DiffHandle(diff);
        collection = loadDiffs(diffHandle, compareOptions);
    }
    catch(const GitException&)
    {
    }

    diffHandle.dispose();
    indexHandle.dispose();

    return collection;
}

DiffDelta::List Diff::diffTreeToWorkDir(const Tree& oldTree, const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    DiffDelta::List collection;
    git_diff* diff = nullptr;

    IndexHandle indexHandle = repository()->index()->createHandle();
    TreeHandle oldTreeHandle = oldTree.createTreeHandle();
    DiffHandle diffHandle;
    try
    {
        if(includeUntracked) {
            diffFlags |= DiffModIncludeUntracked;
        }

        DiffOptions options = buildDiffOptions(diffFlags, paths, compareOptions);

        throwIfTrue(indexHandle.isNull());
        throwIfTrue(oldTreeHandle.isNull());

        throwOnError(git_diff_tree_to_workdir(&diff, repository()->handle().value(), oldTreeHandle.value(), options.toNative()));

        diffHandle = DiffHandle(diff);
        collection = loadDiffs(diffHandle, compareOptions);
    }
    catch(const GitException&)
    {
    }

    diffHandle.dispose();
    indexHandle.dispose();
    oldTreeHandle.dispose();

    return collection;
}

DiffDelta::List Diff::diffTreeToTree_DEP(const Tree& oldTree, const Tree& newTree, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    DiffDelta::List collection;
    git_diff* diff = nullptr;

    IndexHandle indexHandle = repository()->index()->createHandle();
    TreeHandle oldTreeHandle = oldTree.createTreeHandle();
    TreeHandle newTreeHandle = newTree.createTreeHandle();
    DiffHandle diffHandle;

    try
    {
        DiffOptions options = buildDiffOptions(diffFlags, QStringList(), compareOptions);

        throwIfTrue(indexHandle.isNull());
        throwIfTrue(oldTreeHandle.isNull());
        throwIfTrue(newTreeHandle.isNull());

        throwOnError(git_diff_tree_to_tree(&diff, repository()->handle().value(), oldTreeHandle.value(), newTreeHandle.value(), options.toNative()));

        diffHandle = DiffHandle(diff);
        collection = loadDiffs(diffHandle, compareOptions);
    }
    catch(const GitException&)
    {
    }

    diffHandle.dispose();
    indexHandle.dispose();
    return collection;
}

DiffDelta::List Diff::diffCommitToCommit(const Commit& oldCommit, const Commit& newCommit, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    DiffDelta::List result;

    try
    {
        DiffHandle diffHandle = buildDiffList(oldCommit, newCommit, compareOptions, diffFlags);
        throwIfTrue(diffHandle.isNull());
        result = loadDiffs(diffHandle, compareOptions);
        diffHandle.dispose();
    }
    catch(const GitException&)
    {
    }

    return result;
}


DiffDelta::List Diff::loadDiffs(const DiffHandle& handle, const CompareOptions& compareOptions) const
{
    DiffDelta::List collection;
    detectRenames(handle, compareOptions);

    int count = git_diff_num_deltas(handle.value());
    if(count > 0) {
        git_diff_foreach(handle.value(), fileCallback, binaryCallback, hunkCallback, lineCallback, &collection);
    }
    return collection;
}

DiffOptions Diff::buildDiffOptions(DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions) const
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

    if(paths.count() > 0) {
        options.setPaths(paths);
    }

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

DiffHandle Diff::buildDiffList(const Commit& oldCommit, const Commit& newCommit, const CompareOptions& compareOptions, DiffModifiers diffFlags) const
{
    DiffHandle result;
    CommitHandle fromCommitHandle = oldCommit.createHandle();
    CommitHandle toCommitHandle = newCommit.createHandle();

    try
    {
        throwIfTrue(fromCommitHandle.isNull());
        throwIfTrue(toCommitHandle.isNull());

        const git_oid* fromTreeId = git_commit_tree_id(fromCommitHandle.value());
        const git_oid* toTreeId = git_commit_tree_id(toCommitHandle.value());

        git_tree* fromTree = nullptr;
        git_tree* toTree = nullptr;
        throwOnError(git_tree_lookup(&fromTree, repository()->handle().value(), fromTreeId));
        throwOnError(git_tree_lookup(&toTree, repository()->handle().value(), toTreeId));

        DiffOptions options = buildDiffOptions(diffFlags, QStringList(), compareOptions);

        git_diff* diff = nullptr;
        throwOnError(git_diff_tree_to_tree(&diff, repository()->handle().value(), fromTree, toTree, options.toNative()));

        result = DiffHandle(diff);
    }
    catch(const GitException&)
    {
    }

    fromCommitHandle.dispose();
    toCommitHandle.dispose();

    return result;
}

void Diff::detectRenames(const DiffHandle& handle, const CompareOptions& compareOptions) const
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
    if(!compareOptions.includeUnmodified()) {
        opts.flags |= DiffFindRemoveUnmodified;
    }

    git_diff_find_similar(handle.value(), &opts);
}

TreeChanges Diff::buildTreeChanges(const DiffHandle& handle) const
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
