#include "diffoptions.h"

using namespace GIT;

DiffOptions::DiffOptions()
{
    git_diff_options_init(&_native, 1);
}

const git_diff_options* DiffOptions::toNative()
{
    git_diff_options_init(&_native, 1);

    _pathsNative = StringArray(_paths);

    _native.version = 1;
    _native.flags = _flags;
    _native.ignore_submodules = (git_submodule_ignore_t)_ignoreSubmodules;
    _native.pathspec = *_pathsNative.toNative();
    _native.notify_cb = nullptr;
    _native.progress_cb = nullptr;
    _native.payload = nullptr; // TODO
    // _native.payload = _payload.length() == 0
    //                   ? (const void*)nullptr
    //                   : (const void*)_payload.constData();
    _native.context_lines = _contextLines;
    _native.interhunk_lines = _interhunkLines;
    _native.id_abbrev = _idAbbrev;
    _native.max_size = _maxSize;
    _native.old_prefix = _oldPrefixString.isEmpty() ? nullptr : _oldPrefixString.toUtf8().constData();
    _native.new_prefix = _newPrefixString.isEmpty() ? nullptr : _newPrefixString.toUtf8().constData();
    return &_native;
}
