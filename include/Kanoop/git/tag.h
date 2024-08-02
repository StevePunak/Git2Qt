#ifndef TAG_H
#define TAG_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>
#include <QList>

namespace GIT {

class Repository;
class Tag : public GitEntity
{
public:
    Tag(Repository* repo, const QString& name, const ObjectId& objectId);

    QString name() const { return _name; }
    ObjectId objectId() const { return _objectId; }

    virtual bool isNull() const override { return _name.isEmpty(); }

    class List : public QList<Tag> {};

private:
    QString _name;
    ObjectId _objectId;
};

} // namespace GIT

#endif // TAG_H
