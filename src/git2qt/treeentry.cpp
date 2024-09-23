#include "treeentry.h"

#include <gitexception.h>
#include <repository.h>
#include <tree.h>
#include <utility>
#include <utility.h>

using namespace GIT;

TreeEntry::TreeEntry()  :
    GitEntity(TreeEntryEntity, nullptr) {}

TreeEntry::TreeEntry(Repository* repo, const ObjectId& parentTreeId, const ObjectId& objectId, const QString& parentPath) :
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
        _path = Utility::combine(parentPath, _name, true);
        _entryType = (ObjectType)git_tree_entry_type(entry);

        git_object* obj = nullptr;
        throwOnError(git_object_lookup(&obj, repository()->handle().value(), _targetObjectId.toNative(), GIT_OBJECT_ANY));
        _targetType = (ObjectType)git_object_type(obj);
        createTarget();
        git_object_free(obj);
    }
    catch(const GitException&)
    {
    }
    treeHandle.dispose();
}

TreeEntry::TreeEntry(const TreeEntry& other) :
    GitEntity(TreeEntryEntity, other.repository()),
    _parentTreeId(other.parentTreeId())
{
    *this = other;
}

TreeEntry& TreeEntry::operator=(const TreeEntry& other)
{
    GitEntity::operator =(other);
    _parentTreeId = other._parentTreeId;
    _targetObjectId = other._targetObjectId;
    _name = other._name;
    _path = other._path;
    _mode = other._mode;
    _entryType =  other._entryType;
    _targetType = other._targetType;
    if(other._target != nullptr) {
        createTarget();
    }
    return *this;
}

TreeEntry::~TreeEntry()
{
    if(_target != nullptr) {
        delete _target;
    }
}

void TreeEntry::createTarget()
{
    switch(_targetType) {
    case ObjectTypeTree:
        _target = new Tree(repository(), _targetObjectId, _path);
        break;
    case ObjectTypeBlob:
        _target = new Blob(repository(), _targetObjectId);
        break;
    case ObjectTypeCommit:
    case ObjectTypeTag:
    case ObjectTypeDelta:
    case ObjectTypeRefDelta:
        logText(LVL_WARNING, QString("Object type unimplemented (%1)").arg(getObjectTypeString(_targetType)));
        break;
    default:
        break;
    }
}

// ------------------------- TreeEntry::List -------------------------

TreeEntry TreeEntry::List::findByPath(const QString& path) const
{
    TreeEntry result;
    for(const TreeEntry& entry : *this) {
        if(entry.path() == path) {
            result = entry;
            break;
        }
        else if(entry.target() != nullptr && entry.targetType() == ObjectTypeTree) {
            Tree* tree = dynamic_cast<Tree*>(entry.target());
            result = tree->entries().findByPath(path);
            if(result.isValid()) {
                break;
            }
        }
    }
    return result;
}
