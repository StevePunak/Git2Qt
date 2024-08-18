/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_delta object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFDELTA_H
#define DIFFDELTA_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/difffile.h>
#include <Kanoop/git/diffbinary.h>
#include <Kanoop/git/diffline.h>
#include <Kanoop/git/diffhunk.h>

namespace GIT {

class DiffDelta
{
public:
    DiffDelta(const git_diff_delta* delta);

    DeltaType status() const { return _status; }
    DiffDeltaFlags flags() const { return _flags; }
    int similarity() const { return _similarity; }
    int fileCount() const { return _fileCount; }
    DiffFile oldFile() const { return _oldFile; }
    DiffFile newFile() const { return _newFile; }

    DiffBinary::List binaries() const { return _binaries; }
    void appendBinary(const DiffBinary& value) { _binaries.append(value); }

    DiffHunk::List hunks() const { return _hunks; }
    DiffHunk::List& hunksRef() { return _hunks; }
    void appendHunk(const DiffHunk& value) { _hunks.append(value); }

    QString toString() const;

    class List : public QList<DiffDelta>
    {
    public:
        DiffDelta* getDiffDeltaPtr(const DiffDelta& delta)
        {
            DiffDelta* result = nullptr;
            auto it = std::find_if(begin(), end(), [delta](const DiffDelta& mine)
            {
                return delta.oldFile().oid() == mine.oldFile().oid() && delta.newFile().oid() == mine.newFile().oid();
            });
            if(it != end()) {
                result = &(*it);
            }
            return result;
        }
    };


private:
    DeltaType _status = DeltaUnmodified;
    DiffDeltaFlags _flags = (DiffDeltaFlag)0;
    int _similarity = 0;
    int _fileCount = 0;
    DiffFile _oldFile;
    DiffFile _newFile;

    DiffBinary::List _binaries;
    DiffHunk::List _hunks;
};

} // namespace GIT

#endif // DIFFDELTA_H
