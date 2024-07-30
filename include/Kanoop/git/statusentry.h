#ifndef STATUSENTRY_H
#define STATUSENTRY_H

#include <QList>
#include <QString>
#include <Kanoop/git/renamedetails.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class StatusEntry
{
public:
    StatusEntry() {}
    StatusEntry(const QString& filePath, FileStatus status, RenameDetails headToIndexRenameDetails, RenameDetails indexToWorkDirRenameDetails) :
        _filePath(filePath), _status(status), _headToIndexRenameDetails(headToIndexRenameDetails), _indexToWorkDirRenameDetails(indexToWorkDirRenameDetails) {}

    QString filePath() const { return _filePath; }
    FileStatus status() const { return _status; }

    class List : public QList<StatusEntry>
    {
    public:
        List findByStatus(FileStatus status) const
        {
            List result;
            for(const StatusEntry& entry : *this) {
                if(entry.status() == status) {
                    result.append(entry);
                }
            }
            return result;
        }
    };

private:
    QString _filePath;
    FileStatus _status;
    RenameDetails _headToIndexRenameDetails;
    RenameDetails _indexToWorkDirRenameDetails;
};

} // namespace GIT

#endif // STATUSENTRY_H
