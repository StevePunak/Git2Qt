#include "diffbinaryfile.h"

using namespace GIT;

const DiffBinaryFile::DiffBinaryTypeToStringMap DiffBinaryFile::_DiffBinaryTypeToStringMap;

DiffBinaryFile::DiffBinaryFile(const git_diff_binary_file* file) :
    _type((DiffBinaryType)file->type),
    _data(file->data, file->datalen),
    _inflatedlen(file->inflatedlen)
{

}

QString DiffBinaryFile::toString() const
{
    QString output = QString("type: %1  data len: %2  inflated len: %3")
                         .arg(_DiffBinaryTypeToStringMap.getString(_type))
                         .arg(_data.length())
                         .arg(_inflatedlen);
    return output;
}
