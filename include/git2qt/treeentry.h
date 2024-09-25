/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a single entry in a git tree.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TREEENTRY_H
#define TREEENTRY_H
#include <git2qt/gitentity.h>
#include <git2qt/objectid.h>
#include <git2qt/gitobject.h>

namespace GIT {

class Repository;
class Tree;
class GIT2QT_EXPORT TreeEntry : public GitEntity
{
public:
    TreeEntry();
    TreeEntry(Repository* repo, const ObjectId& parentTreeId, const ObjectId& objectId, const QString& parentPath);
    TreeEntry(const TreeEntry& other);
    TreeEntry& operator=(const TreeEntry& other);
    virtual ~TreeEntry();

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

    class GIT2QT_EXPORT List : public QList<TreeEntry>
    {
    public:
        TreeEntry findByPath(const QString& path) const;
    };

private:
    void createTarget();

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
