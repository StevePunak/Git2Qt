/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git tree object
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TREE_H
#define TREE_H
#include <git2qt/gitobject.h>
#include <git2qt/handle.h>
#include <git2qt/treeentry.h>

#include <QMap>

namespace GIT {

class Commit;
class Repository;

class Tree : public GitObject
{
public:
    Tree() :
        GitObject(TreeEntity, nullptr, ObjectId()) {}

    Tree(Repository* repo, const ObjectId& objectId, const QString& path = QString());
    virtual ~Tree();

    static Tree createFromBranchName(Repository* repo, const QString& branchName);
    static Tree createFromCommit(Repository* repo, const Commit& commit);

    TreeEntry findEntryByPath(const QString& path) const;
    TreeEntry::List entries() const { return _entries; }

    ObjectHandle createObjectHandle() const;
    TreeHandle createTreeHandle() const;

    static TreeHandle createTreeHandle(Repository* repo, const ObjectId& treeObjectId);

    class List : public QList<Tree>
    {
    public:
        Tree findByObjectId(const ObjectId& objectId) const
        {
            Tree result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Tree& t) { return t.objectId() == objectId;} );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    QString _path;
    TreeEntry::List _entries;
    Tree::List _trees;
};

} // namespace GIT

#endif // TREE_H
