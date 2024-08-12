#ifndef TREEENTRY_H
#define TREEENTRY_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/gitobject.h>

namespace GIT {

class Repository;
class Tree;
class TreeEntry : public GitEntity
{
public:
    TreeEntry();
    TreeEntry(Repository* repo, const ObjectId& parentTreeId, const ObjectId& objectId);

    ObjectId parentTreeId() const { return _parentTreeId; }
    QString name() const { return _name; }
    QString path() const { return _path; }
    Mode mode() const { return _mode; }
    ObjectType targetType() const { return _targetType; }
    GitObject* target() const { return _target; }
    ObjectType entryType() const { return _entryType; }
    ObjectId targetObjectId() const { return _targetObjectId; }

    bool isValid() const { return _name.isEmpty() == false; }
    virtual bool isNull() const override { return _name.isEmpty(); }

    class List : public QList<TreeEntry>
    {
    public:
        TreeEntry findByPath(const QString& path) const
        {
            TreeEntry result;
            auto it = std::find_if(constBegin(), constEnd(), [path](const TreeEntry& t) { return t.path() == path; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    ObjectId _parentTreeId;
    GitObject* _target = nullptr;
    ObjectId _targetObjectId;
    QString _name;
    QString _path;
    Mode _mode = NonexistentFile;
    ObjectType _entryType = ObjectTypeInvalid;
    ObjectType _targetType = ObjectTypeInvalid;
};

} // namespace GIT

#endif // TREEENTRY_H
