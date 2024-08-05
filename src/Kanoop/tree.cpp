#include "tree.h"

#include <repository.h>

using namespace GIT;

Tree::Tree(Repository* repo, const git_object* treeish) :
    GitObject(TreeEntity, repo, treeish)
{
}

Tree::Tree(Repository* repo, const ObjectId& objectId) :
    GitObject(TreeEntity, repo, objectId)
{
}

Tree::~Tree()
{
}

Tree Tree::createFromBranchName(Repository* repo, const QString& branchName)
{
    Tree result;
    git_object *treeish = nullptr;
    if(git_revparse_single(&treeish, repo->handle().value(), branchName.toUtf8().constData()) == 0) {
        result = Tree(repo, treeish);
    }
    git_object_free(treeish);
    return result;
}

ObjectHandle Tree::createObjectHandle() const
{
    ObjectHandle result;
    git_object* obj = nullptr;
    if(git_object_lookup(&obj, repository()->handle().value(), objectId().toNative(), GIT_OBJECT_TREE) == 0) {
        result = ObjectHandle(obj);
    }
    return result;
}

TreeHandle Tree::createTreeHandle() const
{
    TreeHandle result;
    git_tree* tree = nullptr;
    if(git_tree_lookup(&tree, repository()->handle().value(), objectId().toNative()) == 0) {
        result = TreeHandle(tree);
    }
    return result;
}
