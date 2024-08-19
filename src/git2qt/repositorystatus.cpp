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
    _statusEntries.append(statusEntry);
}
