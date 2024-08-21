/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a single entry in the RepositoryStatus collection.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STATUSENTRY_H
#define STATUSENTRY_H

#include <QList>
#include <QString>
#include <git2qt/renamedetails.h>
#include <git2qt/gittypes.h>

namespace GIT {

class StatusEntry
{
public:
    StatusEntry() {}
    StatusEntry(const QString& filePath, FileStatus status, RenameDetails headToIndexRenameDetails, RenameDetails indexToWorkDirRenameDetails) :
        _filePath(filePath), _status(status), _headToIndexRenameDetails(headToIndexRenameDetails), _indexToWorkDirRenameDetails(indexToWorkDirRenameDetails) {}

    QString path() const { return _filePath; }
    FileStatus status() const { return _status; }

    bool isValid() const { return _filePath.isEmpty() == false; }

    class List : public QList<StatusEntry>
    {
    public:
        StatusEntry findByPath(const QString& path) const
        {
            StatusEntry result;
            auto it = std::find_if(constBegin(), constEnd(), [path](const StatusEntry& e) { return e.path() == path; });
            if(it != constEnd()) {
                return *it;
            }
            return result;
        }

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

        QStringList paths() const
        {
            QStringList result;
            for(const StatusEntry& entry : *this) {
                result.append(entry.path());
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
