/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_diff_hunk object from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef DIFFHUNK_H
#define DIFFHUNK_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/diffline.h>

namespace GIT {

class DiffHunk
{
public:
    DiffHunk() {}
    DiffHunk(const git_diff_hunk* hunk);

    bool operator ==(const DiffHunk& other) const;
    bool operator !=(const DiffHunk& other) const { return !(*this == other); }

    int oldStart() const { return _oldStart; }
    int oldLines() const { return _oldLines; }
    int newStart() const { return _newStart; }
    int newLines() const { return _newLines; }
    QString header() const { return _header; }

    DiffLine::List lines() const { return _lines; }
    void appendLine(const DiffLine& value) { _lines.append(value); }

    QString toString() const;

    class List : public QList<DiffHunk>
    {
    public:
        DiffHunk* getDiffHunkPtr(const DiffHunk& hunk)
        {
            DiffHunk* result = nullptr;
            auto it = std::find_if(begin(), end(), [hunk](const DiffHunk& mine)
            {
               return mine == hunk;
            });
            if(it != end()) {
                result = &(*it);
            }
            return result;
        }
    };

private:
    int _oldStart = 0;
    int _oldLines = 0;
    int _newStart = 0;
    int _newLines = 0;
    QString _header;

    DiffLine::List _lines;
};

} // namespace GIT

#endif // DIFFHUNK_H
