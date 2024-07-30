#include "statusoptions.h"

using namespace GIT;


const git_status_options* GIT::StatusOptions::toNative()
{
    _nativeOptions = GIT_STATUS_OPTIONS_INIT;
    _nativeOptions.show = (git_status_show_t)_show;
    if(_includeIgnored) {
        _nativeOptions.flags |= IncludeIgnored;
    }
    if(_includeUntracked) {
        _nativeOptions.flags |= IncludeUntracked;
    }
    if(_detectRenamesInIndex) {
        _nativeOptions.flags |= RenamesHeadToIndex;
        _nativeOptions.flags |= RenamesFromRewrites;
    }
    if(_detectRenamesInWorkDir) {
        _nativeOptions.flags |= RenamesIndexToWorkDir;
        _nativeOptions.flags |= RenamesFromRewrites;
    }
    if(_excludeSubmodules) {
        _nativeOptions.flags |= ExcludeSubmodules;
    }
    if(_recurseIgnoredDirs) {
        _nativeOptions.flags |= RecurseIgnoredDirs;
    }
    if(_recurseUntrackedDirs) {
        _nativeOptions.flags |= RecurseUntrackedDirs;
    }
    if(_pathSpec.count() > 0) {
        // TODO
    }
    if(_disablePathSpecMatch) {
        _nativeOptions.flags |= DisablePathspecMatch;
    }
    if(_includeUnaltered) {
        _nativeOptions.flags |= IncludeUnmodified;
    }

    return &_nativeOptions;
}
