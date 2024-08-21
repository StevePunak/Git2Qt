#include "treechangeentry.h"

using namespace GIT;

TreeChangeEntry::TreeChangeEntry(const git_diff_delta* delta)
{
    _path = delta->new_file.path;
    _oldPath = delta->old_file.path;

    _mode = (Mode)delta->new_file.mode;
    _oldMode = (Mode)delta->old_file.mode;

    _oid = ObjectId(delta->new_file.id);
    _oldOid = ObjectId(delta->old_file.id);

    _exists = (delta->new_file.flags & GIT_DIFF_FLAG_EXISTS) != 0;
    _oldExists = (delta->old_file.flags & GIT_DIFF_FLAG_EXISTS) != 0;

    _changeKind = getStatusFromChangeKind((ChangeKind)delta->status);
}

ChangeKind TreeChangeEntry::getStatusFromChangeKind(ChangeKind changeKind)
{
    switch (changeKind)
    {
    case ChangeKind::ChangeKindUntracked:
    case ChangeKind::ChangeKindIgnored:
        return ChangeKind::ChangeKindAdded;
    default:
        return changeKind;
    }
}
