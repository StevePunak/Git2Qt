#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <QStringList>
#include <git2.h>

namespace GIT {

class StringArray
{
public:
    StringArray(const QStringList& values);
    virtual ~StringArray();

    char* at(int index) const;
    const git_strarray* native() const { return &_native; }

private:
    git_strarray _native;
};

} // namespace

#endif // STRINGARRAY_H
