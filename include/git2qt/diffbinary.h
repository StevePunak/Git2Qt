/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_binary object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFBINARY_H
#define DIFFBINARY_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/diffbinaryfile.h>

namespace GIT {

class GIT2QT_EXPORT DiffBinary
{
public:
    DiffBinary(const git_diff_binary* binary);

    bool containsData() const { return _containsData; }
    DiffBinaryFile oldFile() const { return _oldFile; }
    DiffBinaryFile newFile() const { return _newFile; }

    QString toString() const;

    class List : public QList<DiffBinary> {};

private:
    bool _containsData = false;
    DiffBinaryFile _oldFile;
    DiffBinaryFile _newFile;
};

} // namespace GIT

#endif // DIFFBINARY_H
