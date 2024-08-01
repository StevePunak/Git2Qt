#ifndef TREE_H
#define TREE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/handle.h>

#include <QMap>

namespace GIT {

class Repository;

class Tree : public GitEntity
{
private:
    Tree() : GitEntity(TreeEntity) {}

public:
    Tree(Repository* repo, git_object* treeish);
    virtual ~Tree();

    static Tree createFromBranchName(Repository* repo, const QString& branchName);

    ObjectId objectId() const { return _objectId; }

    ObjectHandle createObjectHandle() const;
    TreeHandle createTreeHandle() const;

    virtual bool isNull() const override { return _objectId.isValid() == false; }

private:
    ObjectId _objectId;
};

} // namespace GIT

#endif // TREE_H
