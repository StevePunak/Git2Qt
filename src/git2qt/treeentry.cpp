#include "treeentry.h"

#include <gitexception.h>
#include <repository.h>
#include <tree.h>
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

TreeEntry TreeEntry::findForCommit(const Commit& commit, const QString& path)
{
    TreeEntry result;

    CommitHandle commitHandle = commit.createHandle();
    Repository* repo = commit.repository();

    try
    {
        throwIfNull(repo, "No repository");
        throwIfTrue(repo, commitHandle.isNull());

        const git_oid* treeoid = git_commit_tree_id(commitHandle.value());
        throwIfNull(repo, treeoid, "Tree not found for commit");
        ObjectId treeId(treeoid);

        git_tree* tree = nullptr;
        throwOnError(repo, git_tree_lookup(&tree, commit.repository()->handle().value(), treeoid));

        git_tree_entry* entry = nullptr;
        throwOnError(repo, git_tree_entry_bypath(&entry, tree, path.toUtf8().constData()));

        const git_oid* entryoid = git_tree_entry_id(entry);
        throwIfNull(repo, entryoid);
        ObjectId entryId(entryoid);

        if(pathHasParent(path)) {
            TreeHandle parentHandle = findParentTree(repo, TreeHandle(tree), path);
            throwIfTrue(repo, parentHandle.isNull());

            // free the old
            git_tree_free(tree);

            // get the parent
            tree = parentHandle.value();
            treeoid = git_tree_id(parentHandle.value());
            throwIfNull(repo, treeoid, "Tree not found for commit");
            treeId = ObjectId(treeoid);
        }

const git_tree_entry* validate = git_tree_entry_byid(tree, entryId.toNative());
Q_UNUSED(validate);

        QString parentPath = stripFilename(path);
        result = TreeEntry(repo, treeId, entryoid, parentPath);

        git_tree_free(tree);
    }
    catch(const GitException&)
    {
    }

    commitHandle.dispose();

    return result;
}

const GitObject* TreeEntry::target()
{
    if(_target == nullptr) {
        createTarget();
    }
    return _target;
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

TreeHandle TreeEntry::findParentTree(Repository* repo, TreeHandle childHandle, const QString& path)
{
    TreeHandle result;
    try
    {
        QString parentPath = stripFilename(path);

        git_tree_entry* entry = nullptr;
        throwOnError(repo, git_tree_entry_bypath(&entry, childHandle.value(), parentPath.toUtf8().constData()));

        ObjectType type = (ObjectType)git_tree_entry_type(entry);
        throwIfFalse(repo, type == ObjectTypeTree, "Not a tree");

        git_object* obj = nullptr;
        throwOnError(repo, git_tree_entry_to_object(&obj, repo->handle().value(), entry));

        const git_oid* oid = git_object_id(obj);
        throwIfNull(repo, oid, "Failed to lookup tree object id");

        git_tree* tree = nullptr;
        throwOnError(repo, git_tree_lookup(&tree, repo->handle().value(), oid));

        result = TreeHandle(tree);

    }
    catch(const GitException&)
    {}
    return result;
}

QString TreeEntry::stripFilename(const QString& path)
{
    QString result;
    int index = path.lastIndexOf('/');
    if(index > 0) {
        result = path.left(index);
    }
    return result;
}

// ------------------------- TreeEntry::List -------------------------

TreeEntry TreeEntry::List::findByPath(const QString& path)
{
    TreeEntry result;
    for(TreeEntry& entry : *this) {
        if(entry.path() == path) {
            result = entry;
            break;
        }
        else if(entry.target() != nullptr && entry.targetType() == ObjectTypeTree) {
            const Tree* tree = dynamic_cast<const Tree*>(entry.target());
            result = tree->entries().findByPath(path);
            if(result.isValid()) {
                break;
            }
        }
    }
    return result;
}
