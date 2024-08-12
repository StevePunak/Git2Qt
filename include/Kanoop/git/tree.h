#ifndef TREE_H
#define TREE_H
#include <Kanoop/git/gitobject.h>
#include <Kanoop/git/handle.h>
#include <Kanoop/git/treeentry.h>

#include <QMap>

namespace GIT {

class Repository;

class Tree : public GitObject
{
public:
    Tree() :
        GitObject(TreeEntity, nullptr, ObjectId()) {}

    Tree(Repository* repo, const ObjectId& objectId);
    virtual ~Tree();

    static Tree createFromBranchName(Repository* repo, const QString& branchName);

    TreeEntry findEntryByPath(const QString& path) { return _entries.findByPath(path); }
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
    TreeEntry::List _entries;
    Tree::List _trees;
};

} // namespace GIT

#endif // TREE_H
