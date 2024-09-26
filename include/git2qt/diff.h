/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class provides git diff comparison functionality.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFF_H
#define DIFF_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/diffdelta.h>
#include <git2qt/handle.h>

namespace GIT {

class DiffOptions;
class Tree;
class TreeChanges;
class CompareOptions;
class StageOptions;
class Repository;
class GIT2QT_EXPORT Diff : public GitEntity
{
public:
    Diff(Repository* repo) :
        GitEntity(DiffEntity, repo) {}
    virtual ~Diff() {}

    TreeChanges compare(DiffModifiers diffModifiers, const QStringList& paths, const CompareOptions& compareOptions);
    TreeChanges compare(const Tree& fromTree, const Tree& toTree);
    TreeChanges compare(const Tree& fromTree, const Tree& toTree, DiffModifiers diffOptions, const CompareOptions& compareOptions);
    TreeChanges compare(const Tree& oldTree, DiffTargets diffTargets, const QStringList& paths, const CompareOptions& compareOptions);
    GIT::DiffDelta::List diffIndexToWorkDir(const QString& path, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    GIT::DiffDelta::List diffIndexToWorkDir(const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    GIT::DiffDelta::List diffTreeToWorkDir(const Tree& oldTree, const QStringList& paths, bool includeUntracked, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;
    GIT::DiffDelta::List diffTreeToTree(const Tree& oldTree, const Tree& newTree, const CompareOptions& compareOptions, DiffModifiers diffFlags = DiffModifier::DiffModNone) const;

    virtual bool isNull() const override { return false; }

private:
    DiffDelta::List loadDiffs(const DiffHandle& handle, const CompareOptions& compareOptions) const;
    DiffOptions buildDiffOptions(DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions) const;
    DiffHandle buildDiffList(const ObjectId& oldTreeId, DiffModifiers diffOptions, const QStringList& paths, const CompareOptions& compareOptions);
    void detectRenames(const DiffHandle& handle, const CompareOptions& compareOptions) const;
    TreeChanges buildTreeChanges(const DiffHandle& handle);

    // Callbacks
    static int fileCallback(const git_diff_delta *delta, float progress, void *payload);
    static int binaryCallback(const git_diff_delta *d, const git_diff_binary *binary, void *payload);
    static int hunkCallback(const git_diff_delta *d, const git_diff_hunk *h, void *payload);
    static int lineCallback(const git_diff_delta *d, const git_diff_hunk *h, const git_diff_line *l, void *payload);
};

} // namespace GIT

#endif // DIFF_H
