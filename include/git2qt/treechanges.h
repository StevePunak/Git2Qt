/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class IS-A collection of tree changes. It is used
 * by the repository to maintain its tree change information.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TREECHANGES_H
#define TREECHANGES_H
#include <git2qt/gitentity.h>
#include <git2qt/treechangeentry.h>

namespace GIT {

class GIT2QT_EXPORT TreeChanges : public TreeChangeEntry::List
{
public:
    TreeChanges() : TreeChangeEntry::List() {}

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> that have been been added.
    /// </summary>
    TreeChangeEntry::List added() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindAdded);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> that have been deleted.
    /// </summary>
    TreeChangeEntry::List deleted() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindDeleted);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> that have been modified.
    /// </summary>
    TreeChangeEntry::List modified() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindModified);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> which type have been changed.
    /// </summary>
    TreeChangeEntry::List typeChanged() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindTypeChanged);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> which have been renamed
    /// </summary>
    TreeChangeEntry::List renamed() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindRenamed);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> which have been copied
    /// </summary>
    TreeChangeEntry::List copied() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindCopied);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> which are unmodified
    /// </summary>
    TreeChangeEntry::List unmodified() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindUnmodified);
    }

    /// <summary>
    /// List of <see cref="TreeChangeEntry"/> which are conflicted
    /// </summary>
    TreeChangeEntry::List conflicted() const
    {
        return getChangesOfKind(ChangeKind::ChangeKindConflicted);
    }

};

} // namespace GIT

#endif // TREECHANGES_H
