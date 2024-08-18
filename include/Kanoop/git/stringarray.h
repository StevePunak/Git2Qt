/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_strarray structure from libgit2
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <QStringList>
#include <git2.h>

namespace GIT {

class StringArray
{
public:
    StringArray(const QStringList& values);
    StringArray(const QString& value);
    virtual ~StringArray();

    char* at(int index) const;
    int count() const { return _native.count; }

    const git_strarray* native() const { return &_native; }

private:
    void commonInit(const QStringList& values);

    git_strarray _native;
};

} // namespace

#endif // STRINGARRAY_H
