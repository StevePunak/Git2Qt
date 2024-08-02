#include "difffile.h"

using namespace GIT;

DiffFile::DiffFile(const git_diff_file* file) :
    _oid(file->id),
    _path(file->path),
    _size(file->size),
    _flags(file->flags),
    _mode(file->mode)

{
}
