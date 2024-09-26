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
#include <QVariant>
#include <git2qt/renamedetails.h>
#include <git2qt/gittypes.h>

namespace GIT {

class GIT2QT_EXPORT StatusEntry
{
public:
    StatusEntry() {}
    StatusEntry(const QString& filePath, FileStatus status, RenameDetails headToIndexRenameDetails, RenameDetails indexToWorkDirRenameDetails) :
        _filePath(filePath), _status(status), _headToIndexRenameDetails(headToIndexRenameDetails), _indexToWorkDirRenameDetails(indexToWorkDirRenameDetails) {}

    QString path() const { return _filePath; }
    FileStatuses status() const { return _status; }
    RenameDetails headToIndexRenameDetails() const { return _headToIndexRenameDetails; }
    RenameDetails indexToWorkDirRenameDetails() const { return _indexToWorkDirRenameDetails; }

    bool isValid() const { return _filePath.isEmpty() == false; }

    QVariant toVariant() const { return QVariant::fromValue<StatusEntry>(*this); }
    static StatusEntry fromVariant(const QVariant& value) { return value.value<StatusEntry>(); }

    class List : public QList<StatusEntry>
    {
    public:
        void appendIfNotPresent(const StatusEntry::List& entries)
        {
            for(const StatusEntry& entry : entries) {
                appendIfNotPresent(entry);
            }
        }

        void appendIfNotPresent(const StatusEntry& entry)
        {
            if(findByPath(entry.path()).isValid() == false) {
                append(entry);
            }
        }

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
                if(entry.status() & status) {
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
    FileStatuses _status;
    RenameDetails _headToIndexRenameDetails;
    RenameDetails _indexToWorkDirRenameDetails;
};

} // namespace GIT


Q_DECLARE_METATYPE(GIT::StatusEntry)

#endif // STATUSENTRY_H
