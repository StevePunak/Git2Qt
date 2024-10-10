#include "index.h"

#include <diff.h>
#include <gitexception.h>
#include <repository.h>
#include <tree.h>
#include <treechanges.h>

#include "log.h"

using namespace GIT;

Index::Index(Repository* repo) :
    GitEntity(IndexEntity, repo)
{
    reload();
}

Index::~Index()
{
}

void Index::remove(const QString& path)
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_remove_bypath(handle.value(), path.toUtf8().constData());
        handle.dispose();
    }
}

void Index::add(const QString& path)
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_add_bypath(handle.value(), path.toUtf8().constData());
        handle.dispose();
    }
}

void Index::add(const QString& path, const ObjectId& objectId, Mode mode)
{
    IndexHandle handle = createHandle();
    try
    {
        throwIfTrue(handle.isNull());
        git_index_entry entry;
        memset(&entry, 0, sizeof(entry));
        entry.mode = mode;
        memcpy(&entry.id, objectId.toNative(), sizeof(entry.id));
        entry.path = path.toUtf8().constData();

        throwOnError(git_index_add(handle.value(), &entry));
    }
    catch(const GitException&)
    {
    }
    handle.dispose();
}

void Index::replace(const Commit& commit, const QStringList& paths, const CompareOptions& compareOptions)
{
    CompareOptions options = compareOptions;
    options.setSimilarity(SimilarityOptions::none());
    TreeChanges changes = repository()->diff()->compare(commit.tree(), DiffTargetIndex, paths, options);
    replace(changes);
}

void Index::replace(const TreeChanges& changes)
{
    for(const TreeChangeEntry& change : changes) {
        switch(change.changeKind()) {
        case ChangeKindUnmodified:
            break;

        case ChangeKindAdded:
            remove(change.path());
            break;

        case ChangeKindDeleted:
        case ChangeKindModified:
            add(change.oldPath(), change.oldOid(), change.oldMode());
            break;

        default:
            Log::logText(LVL_ERROR, QString("Entry '%1' has an unexpected change kind %2").arg(change.path()).arg(getChangeKindString(change.changeKind())));
            break;
        }
    }
}

void Index::write()
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_write(handle.value());
        handle.dispose();
    }
}

ObjectId Index::writeTree()
{
    ObjectId result;
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_oid oid;
        if(git_index_write_tree(&oid, handle.value()) == 0) {
            result = ObjectId(oid);
        }
        handle.dispose();
    }
    return result;
}

void Index::reload()
{
    _entries.clear();
    IndexHandle handle = createHandle();
    git_index_iterator* it = nullptr;
    try
    {
        throwIfTrue(handle.isNull());
        throwOnError(git_index_iterator_new(&it, handle.value()));
        const git_index_entry* nativeEntry = nullptr;
        while(git_index_iterator_next(&nativeEntry, it) == 0) {
            QString path = nativeEntry->path;
            ObjectId objectId = nativeEntry->id;
            StageLevel stageLevel = (StageLevel)git_index_entry_stage(nativeEntry);
            Mode mode = (Mode)nativeEntry->mode;
            bool assumeUnchanged = (nativeEntry->flags & GIT_IDXENTRY_VALID) != 0;

            IndexEntry entry(path, mode, stageLevel, assumeUnchanged, objectId);
            _entries.append(entry);
        }
    }
    catch(const GitException&)
    {
    }

    handle.dispose();
    if(it != nullptr) {
        git_index_iterator_free(it);
    }
}

IndexHandle Index::createHandle() const
{
    IndexHandle result;
    git_index* index = nullptr;
    if(git_repository_index(&index, repository()->handle().value()) == 0) {
        result = IndexHandle(index);
    }
    return result;
}

bool Index::isNull() const
{
    IndexHandle handle = createHandle();
    bool result = handle.isNull();
    handle.dispose();
    return result;
    return createHandle().isNull();
}

bool GIT::Index::isFullyMerged() const
{
    bool result = false;
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        result = git_index_has_conflicts(handle.value()) == 0;
    }
    return result;
}
