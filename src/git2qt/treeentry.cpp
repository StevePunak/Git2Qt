#include "treeentry.h"

#include <gitexception.h>
#include <repository.h>
#include <tree.h>

using namespace GIT;

TreeEntry::TreeEntry()  :
    GitEntity(TreeEntryEntity, nullptr) {}

TreeEntry::TreeEntry(Repository* repo, const ObjectId& parentTreeId, const ObjectId& objectId) :
    GitEntity(TreeEntryEntity, repo),
    _parentTreeId(parentTreeId)
{
    TreeHandle treeHandle = Tree::createTreeHandle(repository(), parentTreeId);
    try
    {
        throwIfTrue(treeHandle.isNull());
        const git_tree_entry* entry = git_tree_entry_byid(treeHandle.value(), objectId.toNative());
        throwIfNull(entry);
        _mode = (Mode)git_tree_entry_filemode(entry);
        _targetObjectId = git_tree_entry_id(entry);
        _name = git_tree_entry_name(entry);
        _entryType = (ObjectType)git_tree_entry_type(entry);

        git_object* obj = nullptr;
        throwOnError(git_object_lookup(&obj, repository()->handle().value(), _targetObjectId.toNative(), GIT_OBJECT_ANY));
        _targetType = (ObjectType)git_object_type(obj);
        git_object_free(obj);
    }
    catch(const GitException&)
    {
    }
    treeHandle.dispose();
}
