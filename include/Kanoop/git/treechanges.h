#ifndef TREECHANGES_H
#define TREECHANGES_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/treeentrychanges.h>

namespace GIT {

class TreeChanges : public TreeEntryChanges::List
{
public:
    TreeChanges() : TreeEntryChanges::List() {}

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> that have been been added.
    /// </summary>
    TreeEntryChanges::List added() const
    {
        return getChangesOfKind(ChangeKind::Added);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> that have been deleted.
    /// </summary>
    TreeEntryChanges::List deleted() const
    {
        return getChangesOfKind(ChangeKind::Deleted);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> that have been modified.
    /// </summary>
    TreeEntryChanges::List modified() const
    {
        return getChangesOfKind(ChangeKind::Modified);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> which type have been changed.
    /// </summary>
    TreeEntryChanges::List typeChanged() const
    {
        return getChangesOfKind(ChangeKind::TypeChanged);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> which have been renamed
    /// </summary>
    TreeEntryChanges::List renamed() const
    {
        return getChangesOfKind(ChangeKind::Renamed);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> which have been copied
    /// </summary>
    TreeEntryChanges::List copied() const
    {
        return getChangesOfKind(ChangeKind::Copied);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> which are unmodified
    /// </summary>
    TreeEntryChanges::List unmodified() const
    {
        return getChangesOfKind(ChangeKind::Unmodified);
    }

    /// <summary>
    /// List of <see cref="TreeEntryChanges"/> which are conflicted
    /// </summary>
    TreeEntryChanges::List conflicted() const
    {
        return getChangesOfKind(ChangeKind::Conflicted);
    }

};

} // namespace GIT

#endif // TREECHANGES_H
