/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class provides git diff comparison functionality.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFF_H
#define DIFF_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/diffdelta.h>
#include <Kanoop/git/handle.h>

namespace GIT {

class Tree;
class TreeChanges;
class CompareOptions;
class StageOptions;
class Repository;
class Diff : public GitEntity
{
public:
    Diff(Repository* repo) :
        GitEntity(DiffEntity, repo) {}
    virtual ~Diff() {}

    TreeChanges compare(DiffModifiers diffModifiers, const QStringList& paths, const CompareOptions& compareOptions);
    TreeChanges compare(const Tree& fromTree, const Tree& toTree);
    TreeChanges compare(const Tree& fromTree, const Tree& toTree, DiffModifiers diffOptions, const CompareOptions& compareOptions);
    TreeChanges compare(const Tree& oldTree, DiffTargets diffTargets, const QStringList& paths, const CompareOptions& compareOptions);
    GIT::DiffDelta::List listDiffs(const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone);

    virtual bool isNull() const override { return false; }

private:
    void buildDiffOptions(DiffModifiers diffOptions, const CompareOptions& compareOptions, git_diff_options& options);
    DiffHandle buildDiffList(const ObjectId& oldTreeId, DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions);
    void detectRenames(const DiffHandle& handle, const CompareOptions& compareOptions);
    TreeChanges buildTreeChanges(const DiffHandle& handle);

    // Callbacks
    static int fileCallback(const git_diff_delta *delta, float progress, void *payload);
    static int binaryCallback(const git_diff_delta *d, const git_diff_binary *binary, void *payload);
    static int hunkCallback(const git_diff_delta *d, const git_diff_hunk *h, void *payload);
    static int lineCallback(const git_diff_delta *d, const git_diff_hunk *h, const git_diff_line *l, void *payload);
};

} // namespace GIT

#endif // DIFF_H
