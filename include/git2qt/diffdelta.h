/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_delta object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFDELTA_H
#define DIFFDELTA_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/difffile.h>
#include <git2qt/diffbinary.h>
#include <git2qt/diffline.h>
#include <git2qt/diffhunk.h>

namespace GIT {

class GIT2QT_EXPORT DiffDelta
{
public:
    DiffDelta() {}
    DiffDelta(const git_diff_delta* delta);

    DeltaType status() const { return _status; }
    DiffDeltaFlags flags() const { return _flags; }
    int similarity() const { return _similarity; }
    int fileCount() const { return _fileCount; }

    DiffFile oldFile() const { return _oldFile; }
    void setOldFile(const DiffFile& value) { _oldFile = value; }

    DiffFile newFile() const { return _newFile; }
    void setNewFile(const DiffFile& value) { _newFile = value; }

    DiffBinary::List binaries() const { return _binaries; }
    void appendBinary(const DiffBinary& value) { _binaries.append(value); }

    DiffHunk::List hunks() const { return _hunks; }
    DiffHunk::List& hunksRef() { return _hunks; }
    void appendHunk(const DiffHunk& value) { _hunks.append(value); }

    DiffHunk findHunkForOldLine(int line) const { return _hunks.findHunkForOldLine(line); }
    DiffHunk findHunkForNewLine(int line) const { return _hunks.findHunkForNewLine(line); }

    bool isValid() const { return _oldFile.isValid() || _newFile.isValid(); }

    QString toString() const;

    class List : public QList<DiffDelta>
    {
    public:
        DiffDelta findFirstByPath(const QString& filePath) const
        {
            DiffDelta result;
            auto it = std::find_if(constBegin(), constEnd(), [filePath](const DiffDelta& delta)
            {
                return delta.newFile().path() == filePath || delta.oldFile().path() == filePath;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        DiffDelta findFirstByOldFileId(const ObjectId& objectId) const
        {
            DiffDelta result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const DiffDelta& delta)
            {
                return delta.oldFile().objectId() == objectId;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        DiffDelta* getDiffDeltaPtr(const DiffDelta& delta)
        {
            DiffDelta* result = nullptr;
            auto it = std::find_if(begin(), end(), [delta](const DiffDelta& mine)
            {
                return delta.oldFile().objectId() == mine.oldFile().objectId() && delta.newFile().objectId() == mine.newFile().objectId();
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
