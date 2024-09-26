/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is a port of the StageOptions class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STAGEOPTIONS_H
#define STAGEOPTIONS_H
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT StageOptions
{
public:
    StageOptions() :
        _includeIgnored(false) {}

    bool includeIgnored() const { return _includeIgnored; }
    void setIncludeIgnored(bool value) { _includeIgnored = value; }

private:
    bool _includeIgnored;
};

} // namespace GIT

#endif // STAGEOPTIONS_H
