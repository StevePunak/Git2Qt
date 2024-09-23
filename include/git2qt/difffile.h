/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_file object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFFILE_H
#define DIFFFILE_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>

namespace GIT {

class DiffFile
{
public:
    DiffFile() {}
    DiffFile(const git_diff_file* file);

    ObjectId objectId() const { return _oid; }
    QString path() const { return _path; }
    int size() const { return _size; }
    DiffDeltaFlags flags() const { return _flags; }
    uint16_t mode() const { return _mode; }

    bool isValid() const { return _path.isEmpty() == false; }

private:
    ObjectId _oid;
    QString _path;
    int _size = 0;
    DiffDeltaFlags _flags = (DiffDeltaFlag)0;
    uint16_t _mode = 0;
};

} // namespace GIT

#endif // DIFFFILE_H
