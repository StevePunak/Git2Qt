/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Represents a single entry in an Index.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef INDEXENTRY_H
#define INDEXENTRY_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>

#include <QList>
#include <QVariant>

namespace GIT {

class IndexEntry
{
public:
    IndexEntry() :
        _mode(NonexistentFile), _stageLevel(Staged), _assumeUnchanged(false) {}
    IndexEntry(const QString& path, Mode mode, StageLevel stageLevel, bool assumeUnchanged, const ObjectId& objectId);

    QString path() const { return _path; }
    Mode mode() const { return _mode; }
    StageLevel stageLevel() const { return _stageLevel; }
    bool assumeUnchanged() const { return _assumeUnchanged; }
    ObjectId objectId() const { return _objectId; }

    bool isValid() const { return _objectId.isValid(); }

    QVariant toVariant() const { return QVariant::fromValue<IndexEntry>(*this); }
    static IndexEntry fromVariant(const QVariant& value) { return value.value<IndexEntry>(); }

    class List : public QList<IndexEntry>
    {
    public:
        IndexEntry findByPath(const QString& path) const
        {
            IndexEntry result;
            auto it = std::find_if(constBegin(), constEnd(), [path](const IndexEntry& e) { return e.path() == path; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    QString _path;
    Mode _mode;
    StageLevel _stageLevel;
    bool _assumeUnchanged;
    ObjectId _objectId;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::IndexEntry)

#endif // INDEXENTRY_H
