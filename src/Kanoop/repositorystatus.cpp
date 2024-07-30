#include "repositorystatus.h"

using namespace GIT;


void RepositoryStatus::addStatusEntryForDelta(FileStatus fileStatus, git_diff_delta* deltaHeadToIndex, git_diff_delta* deltaIndexToWorkDir)
{
    RenameDetails headToIndexRenameDetails;
    RenameDetails indexToWorkDirRenameDetails;

    if (fileStatus & FileStatus::RenamedInIndex) {
        headToIndexRenameDetails.setOldFilePath(deltaHeadToIndex->old_file.path);
        headToIndexRenameDetails.setNewFilePath(deltaHeadToIndex->new_file.path);
        headToIndexRenameDetails.setSimilarity(deltaHeadToIndex->similarity);
    }

    if (fileStatus & FileStatus::RenamedInWorkdir) {
        indexToWorkDirRenameDetails.setOldFilePath(deltaIndexToWorkDir->old_file.path);
        indexToWorkDirRenameDetails.setNewFilePath(deltaIndexToWorkDir->new_file.path);
        indexToWorkDirRenameDetails.setSimilarity(deltaIndexToWorkDir->similarity);
    }

    QString filePath = deltaIndexToWorkDir != nullptr
                           ? deltaIndexToWorkDir->new_file.path
                           : deltaHeadToIndex->new_file.path;

    StatusEntry statusEntry(filePath, fileStatus, headToIndexRenameDetails, indexToWorkDirRenameDetails);

    if (fileStatus == FileStatus::Unaltered) {
        _unaltered.append(statusEntry);
    }
    else {
        if(fileStatus & FileStatus::NewInWorkdir) {
            _untracked.append(statusEntry);
        }
        if(fileStatus & FileStatus::ModifiedInWorkdir) {
            _modified.append(statusEntry);
        }
        if(fileStatus & FileStatus::DeletedFromWorkdir) {
            _missing.append(statusEntry);
        }
        if(fileStatus & FileStatus::NewInIndex) {
            _added.append(statusEntry);
        }
        if(fileStatus & FileStatus::ModifiedInIndex) {
            _staged.append(statusEntry);
        }
        if(fileStatus & FileStatus::DeletedFromIndex) {
            _removed.append(statusEntry);
        }
        if(fileStatus & FileStatus::RenamedInIndex) {
            _renamedInIndex.append(statusEntry);
        }
        if(fileStatus & FileStatus::Ignored) {
            _ignored.append(statusEntry);
        }
        if(fileStatus & FileStatus::RenamedInWorkdir) {
            _renamedInWorkDir.append(statusEntry);
        }
    }

    _statusEntries.append(statusEntry);
}
