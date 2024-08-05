#ifndef TREE_H
#define TREE_H
#include <Kanoop/git/gitobject.h>
#include <Kanoop/git/handle.h>

#include <QMap>

namespace GIT {

class Repository;

class Tree : public GitObject
{
private:
    Tree() :
        GitObject(TreeEntity, nullptr, ObjectId()) {}

public:
    Tree(Repository* repo, const git_object* treeish);
    Tree(Repository* repo, const ObjectId& objectId);
    virtual ~Tree();

    static Tree createFromBranchName(Repository* repo, const QString& branchName);

    ObjectHandle createObjectHandle() const;
    TreeHandle createTreeHandle() const;
};

} // namespace GIT

#endif // TREE_H
