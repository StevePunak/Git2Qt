#ifndef TREE_H
#define TREE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/objectid.h>

#include <QMap>

namespace GIT {

class Repository;

class Tree : public GitEntity
{
public:
    explicit Tree(Repository* repo, git_object* treeish);
    virtual ~Tree();

    static Tree* createFromBranchName(Repository* repo, const QString& branchName);

    ObjectId objectId() const { return _objectId; }

    git_object *objectHandle() const { return _handle; }
    git_tree *treeHandle() const { return _tree; }

    virtual bool isNull() const override { return _handle == nullptr; }

private:
    ObjectId _objectId;
    git_object *_handle = nullptr;
    git_tree *_tree = nullptr;
};

} // namespace GIT

#endif // TREE_H
