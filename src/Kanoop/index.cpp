#include "index.h"

#include <repository.h>

using namespace GIT;

Index::Index(Repository* repo) :
    GitEntity(IndexEntity, repo)
{
}

Index::~Index()
{
}

void Index::remove(const QString& path)
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_remove_bypath(handle.value(), path.toUtf8().constData());
    }
}

void Index::add(const QString& path)
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_add_bypath(handle.value(), path.toUtf8().constData());
    }
}

void Index::write()
{
    IndexHandle handle = createHandle();
    if(handle.isNull() == false) {
        git_index_write(handle.value());
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
    }
    return result;
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
