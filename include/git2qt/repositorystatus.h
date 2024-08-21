/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class contains the status of all files in the git repository.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REPOSITORYSTATUS_H
#define REPOSITORYSTATUS_H
#include <git2qt/gittypes.h>
#include <git2qt/statusentry.h>
#include <git2.h>

namespace GIT {

class RepositoryStatus
{
public:
    RepositoryStatus() :
        _dirty(false) {}

    void addStatusEntryForDelta(FileStatus fileStatus, git_diff_delta* deltaHeadToIndex, git_diff_delta* deltaIndexToWorkDir);

    StatusEntry::List entries() const { return _statusEntries; }
    StatusEntry::List added() const { return _statusEntries.findByStatus(NewInIndex); }
    StatusEntry::List staged() const { return _statusEntries.findByStatus(ModifiedInIndex); }
    StatusEntry::List removed() const { return _statusEntries.findByStatus(DeletedFromIndex); }
    StatusEntry::List missing() const { return _statusEntries.findByStatus(DeletedFromWorkdir); }
    StatusEntry::List modified() const { return _statusEntries.findByStatus(ModifiedInWorkdir); }
    StatusEntry::List untracked() const { return _statusEntries.findByStatus(NewInWorkdir); }
    StatusEntry::List ignored() const { return _statusEntries.findByStatus(Ignored); }
    StatusEntry::List renamedInIndex() const { return _statusEntries.findByStatus(RenamedInIndex); }
    StatusEntry::List renamedInWorkDir() const { return _statusEntries.findByStatus(RenamedInWorkdir); }
    StatusEntry::List unaltered() const { return _statusEntries.findByStatus(Unaltered); }

    bool isDirty() const { return _dirty; }

private:
    StatusEntry::List _statusEntries;
    bool _dirty;
};

} // namespace GIT

#endif // REPOSITORYSTATUS_H
