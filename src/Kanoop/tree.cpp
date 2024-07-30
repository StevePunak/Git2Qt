#include "tree.h"

#include <repository.h>

using namespace GIT;

Tree::Tree(Repository* repo, git_object* treeish) :
    GitEntity(TreeEntity, repo),
    _handle(treeish)
{
    const git_oid* oid = git_object_id(treeish);
    if(oid != nullptr) {
        _objectId = ObjectId(oid);
        if(git_tree_lookup(&_tree, repo->handle(), oid) != 0) {
            _tree = nullptr;
        }
    }
}

Tree::~Tree()
{
    if(_handle != nullptr) {
        git_object_free(_handle);
    }
    if(_tree != nullptr) {
        git_tree_free(_tree);
    }
}

Tree* Tree::createFromBranchName(Repository* repo, const QString& branchName)
{
    Tree* result = nullptr;
    git_object *treeish = nullptr;
    if(git_revparse_single(&treeish, repo->handle(), branchName.toUtf8().constData()) == 0) {
        result = new Tree(repo, treeish);
    }
    return result;
}
