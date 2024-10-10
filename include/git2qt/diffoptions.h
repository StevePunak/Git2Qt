/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_options structure from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFOPTIONS_H
#define DIFFOPTIONS_H
#include <git2qt/gittypes.h>
#include <git2.h>
#include <git2qt/stringarray.h>

namespace GIT {

class GIT2QT_EXPORT DiffOptions
{
public:
    DiffOptions();

    DiffOptionFlags flags() const { return _flags; }
    void setFlags(DiffOptionFlags flags) { _flags |= flags; }

    SubmoduleIgnore ignoreSubmodules() const { return _ignoreSubmodules; }
    void setIgnoreSubmodules(SubmoduleIgnore value) { _ignoreSubmodules = value; }

    QStringList paths() const { return _paths; }
    void setPaths(const QStringList& value) { _paths = value; }

    QByteArray payload() const { return _payload; }
    void setPayload(const QByteArray& value) { _payload = value; }

    int contextLines() const { return _contextLines; }
    void setContextLines(int value) { _contextLines = value; }

    int interhunkLines() const { return _interhunkLines; }
    void setInterhunkLines(int value) { _interhunkLines = value; }

    int maxSize() const { return _maxSize; }
    void setMaxSize(int value) { _maxSize = value; }

    uint16_t idAbbrev() const { return _idAbbrev; }
    void setIdAbbrev(uint16_t value) { _idAbbrev = value; }

    QString oldPrefixString() const { return _oldPrefixString; }
    void setOldPrefixString(const QString& value) { _oldPrefixString = value; }

    QString newPrefixString() const { return _newPrefixString; }
    void setNewPrefixString(const QString& value) { _newPrefixString = value; }

    const git_diff_options* toNative();

private:
    DiffOptionFlags _flags;
    SubmoduleIgnore _ignoreSubmodules = SubmoduleIgnore::Reset;
    QStringList _paths;
    // TODO notify callback
    // TODO progress callback
    QByteArray _payload;
    int _contextLines = 0;
    int _interhunkLines = 0;
    int _maxSize = 0;
    uint16_t _idAbbrev = 0;
    QString _oldPrefixString;
    QString _newPrefixString;

    StringArray _pathsNative;
    git_diff_options _native;
};

} // namespace GIT

#endif // DIFFOPTIONS_H
