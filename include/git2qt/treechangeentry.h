/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a change in a git tree
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TREEENTRYCHANGES_H
#define TREEENTRYCHANGES_H

#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>

#include <QList>
#include <QString>


namespace GIT {

class TreeChangeEntry
{
public:
    TreeChangeEntry(const git_diff_delta* delta);

    QString path() const { return _path; }
    QString oldPath() const { return _oldPath; }
    Mode mode() const { return _mode; }
    Mode oldMode() const { return _oldMode; }
    ObjectId oid() const { return _oid; }
    ObjectId oldOid() const { return _oldOid; }
    bool exists() const { return _exists; }
    bool oldExists() const { return _oldExists; }
    ChangeKind changeKind() const { return _changeKind; }

    class List : public QList<TreeChangeEntry>
    {
    public:
        List getChangesOfKind(ChangeKind kind) const
        {
            List result;
            for(const TreeChangeEntry& change : *this) {
                if(change.changeKind() == kind) {
                    result.append(change);
                }
            }
            return result;
        }

        List getChangesNotOfKinds(QList<ChangeKind> kinds) const
        {
            List result;
            for(const TreeChangeEntry& change : *this) {
                if(kinds.contains(change.changeKind()) == false) {
                    result.append(change);
                }
            }
            return result;
        }
    };

private:
    static ChangeKind getStatusFromChangeKind(ChangeKind changeKind);

    QString _path;
    QString _oldPath;
    Mode _mode;
    Mode _oldMode;
    ObjectId _oid;
    ObjectId _oldOid;
    bool _exists;
    bool _oldExists;
    ChangeKind _changeKind;
};

} // namespace GIT

#endif // TREEENTRYCHANGES_H
