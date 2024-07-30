#ifndef TREEENTRYCHANGES_H
#define TREEENTRYCHANGES_H

#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>

#include <QList>
#include <QString>


namespace GIT {

class TreeEntryChanges
{
public:
    TreeEntryChanges(const git_diff_delta* delta);

    QString path() const { return _path; }
    QString oldPath() const { return _oldPath; }
    Mode mode() const { return _mode; }
    Mode oldMode() const { return _oldMode; }
    ObjectId oid() const { return _oid; }
    ObjectId oldOid() const { return _oldOid; }
    bool exists() const { return _exists; }
    bool oldExists() const { return _oldExists; }
    ChangeKind::Kind status() const { return _status; }

    class List : public QList<TreeEntryChanges>
    {
    public:
        List getChangesOfKind(ChangeKind::Kind kind) const
        {
            List result;
            for(const TreeEntryChanges& change : *this) {
                if(change.status() == kind) {
                    result.append(change);
                }
            }
            return result;
        }

        List getChangesNotOfKinds(QList<ChangeKind::Kind> kinds) const
        {
            List result;
            for(const TreeEntryChanges& change : *this) {
                if(kinds.contains(change.status()) == false) {
                    result.append(change);
                }
            }
            return result;
        }
    };

private:
    static ChangeKind::Kind getStatusFromChangeKind(ChangeKind::Kind changeKind);

    QString _path;
    QString _oldPath;
    Mode _mode;
    Mode _oldMode;
    ObjectId _oid;
    ObjectId _oldOid;
    bool _exists;
    bool _oldExists;
    ChangeKind::Kind _status;
};

} // namespace GIT

#endif // TREEENTRYCHANGES_H
