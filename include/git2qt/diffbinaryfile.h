/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_binary_file object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFBINARYFILE_H
#define DIFFBINARYFILE_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/enumtostringmap.h>

namespace GIT {

class DiffBinaryFile
{
public:
    enum DiffBinaryType
    {
        /** There is no binary delta. */
        None = GIT_DIFF_BINARY_NONE,

        /** The binary data is the literal contents of the file. */
        Literal = GIT_DIFF_BINARY_LITERAL,

        /** The binary data is the delta from one side to the other. */
        Delta = GIT_DIFF_BINARY_DELTA,
    };

    DiffBinaryFile(const git_diff_binary_file* file);

    QString toString() const;

private:
    DiffBinaryType _type;
    QByteArray _data;
    int _inflatedlen;

    class DiffBinaryTypeToStringMap : public GIT::EnumToStringMap<DiffBinaryType>
    {
    public:
        DiffBinaryTypeToStringMap()
        {
            insert(None,            "None");
            insert(Literal,         "Literal");
            insert(Delta,           "Delta");
        }
    };
    \
    static const DiffBinaryTypeToStringMap _DiffBinaryTypeToStringMap;

};

} // namespace GIT

#endif // DIFFBINARYFILE_H
