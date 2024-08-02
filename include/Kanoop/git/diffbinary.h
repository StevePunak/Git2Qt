#ifndef DIFFBINARY_H
#define DIFFBINARY_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/diffbinaryfile.h>

namespace GIT {

class DiffBinary
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
