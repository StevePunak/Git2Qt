/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is a port of the StatusOptions class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STATUSOPTIONS_H
#define STATUSOPTIONS_H
#include <QStringList>
#include <git2.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class StatusOptions
{
public:
    StatusOptions() {}

    bool detectRenamesInIndex() const { return _detectRenamesInIndex; }
    void setDetectRenamesInIndex(bool value) { _detectRenamesInIndex = value; }

    bool includeIgnored() const { return _includeIgnored; }
    void setIncludeIgnored(bool value) { _includeIgnored = value; }

    bool includeUntracked() const { return _includeUntracked; }
    void setIncludeUntracked(bool value) { _includeUntracked = value; }

    bool recurseUntrackedDirs() const { return _recurseUntrackedDirs; }
    void setRecurseUntrackedDirs(bool value) { _recurseUntrackedDirs = value; }

    GitStatusShow show() const { return _show; }
    void setShow(GitStatusShow value) { _show = value; }

    bool detectRenamesInWorkDir() const { return _detectRenamesInWorkDir; }
    void setDetectRenamesInWorkDir(bool value) { _detectRenamesInWorkDir = value; }

    bool excludeSubmodules() const { return _excludeSubmodules; }
    void setExcludeSubmodules(bool value) { _excludeSubmodules = value; }

    bool recurseIgnoredDirs() const { return _recurseIgnoredDirs; }
    void setRecurseIgnoredDirs(bool value) { _recurseIgnoredDirs = value; }

    QStringList pathSpec() const { return _pathSpec; }
    void setPathSpec(QStringList value) { _pathSpec = value; }

    bool disablePathSpecMatch() const { return _disablePathSpecMatch; }
    void setDisablePathSpecMatch(bool value) { _disablePathSpecMatch = value; }

    bool includeUnaltered() const { return _includeUnaltered; }
    void setIncludeUnaltered(bool value) { _includeUnaltered = value; }

    const git_status_options* toNative();

private:
    bool _detectRenamesInIndex = true;
    bool _includeIgnored = false;
    bool _includeUntracked = true;
    bool _recurseUntrackedDirs = true;
    GitStatusShow _show = IndexAndWorkDir;
    bool _detectRenamesInWorkDir = true;
    bool _excludeSubmodules = true;
    bool _recurseIgnoredDirs = false;
    QStringList _pathSpec;
    bool _disablePathSpecMatch = false;
    bool _includeUnaltered = false;

    git_status_options _nativeOptions;
};

} // namespace GIT

#endif // STATUSOPTIONS_H
