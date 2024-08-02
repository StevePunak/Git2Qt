#ifndef DIFFFILE_H
#define DIFFFILE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>

namespace GIT {

class DiffFile
{
public:
    DiffFile(const git_diff_file* file);

    ObjectId oid() const { return _oid; }
    QString path() const { return _path; }
    int size() const { return _size; }
    DiffDeltaFlags flags() const { return _flags; }
    uint16_t mode() const { return _mode; }

private:
    ObjectId _oid;
    QString _path;
    int _size = 0;
    DiffDeltaFlags _flags = (DiffDeltaFlag)0;
    uint16_t _mode = 0;
};

} // namespace GIT

#endif // DIFFFILE_H
