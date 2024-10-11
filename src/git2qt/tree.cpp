#include "tree.h"

#include <gitexception.h>
#include <repository.h>

#include "log.h"

using namespace GIT;

Tree::Tree(Repository* repo, const ObjectId& objectId, const QString& path) :
    GitObject(TreeEntity, repo, objectId),
    _path(path)
{
    try
    {
        git_object* obj = nullptr;
        throwOnError(git_object_lookup(&obj, repository()->handle().value(), objectId.toNative(), GIT_OBJECT_TREE));
        git_object_free(obj);

        TreeHandle handle = createTreeHandle();
        throwIfTrue(handle.isNull());
        size_t count = git_tree_entrycount(handle.value());
logText(LVL_DEBUG, QString("Create tree at %1 with %2 entries").arg(path).arg(count));
        for(size_t i = 0;i < count;i++) {
            const git_tree_entry* nativeEntry = git_tree_entry_byindex(handle.value(), i);
            throwIfNull(nativeEntry);
            ObjectId entryId = git_tree_entry_id(nativeEntry);
            TreeEntry entry(repository(), objectId, entryId, path);
            _entries.append(entry);
        }
        handle.dispose();
    }
    catch(const GitException&)
    {
    }
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

Tree Tree::createFromCommit(Repository* repo, const Commit& commit)
{
    Tree result;
    CommitHandle commitHandle = commit.createHandle();
    git_tree *tree = nullptr;
    try
    {
        throwIfTrue(repo, commitHandle.isNull());
        throwOnError(repo, git_commit_tree(&tree, commitHandle.value()));
        const git_oid* oid = git_tree_id(tree);
        throwIfNull(repo, oid);
        result = Tree(repo, ObjectId(oid));
    }
    catch(const GitException&)
    {
    }

    if(tree != nullptr) {
        git_tree_free(tree);
    }
    return result;
}

TreeEntry Tree::findEntryByPath(const QString& path) const
{
    return _entries.findByPath(path);
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

TreeHandle Tree::createTreeHandle(Repository* repo, const ObjectId& treeObjectId)
{
    TreeHandle result;
    git_tree* tree = nullptr;
    if(git_tree_lookup(&tree, repo->handle().value(), treeObjectId.toNative()) == 0) {
        result = TreeHandle(tree);
    }
    return result;
}

