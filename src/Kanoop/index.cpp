#include "index.h"

#include <repository.h>

using namespace GIT;

Index::Index(Repository* repo) :
    GitEntity(IndexEntity, repo),
    _handle(nullptr)
{
    if(git_repository_index(&_handle, repo->handle())) {
        _handle = nullptr;
    }
}

Index::~Index()
{
    if(_handle != nullptr) {
        git_index_free(_handle);
    }
}

void Index::remove(const QString& path)
{
    git_index_remove_bypath(_handle, path.toUtf8().constData());
}

void Index::add(const QString& path)
{
    git_index_add_bypath(_handle, path.toUtf8().constData());
}

void Index::write()
{
    git_index_write(_handle);
}

ObjectId Index::writeTree()
{
    ObjectId result;
    git_oid oid;
    if(git_index_write_tree(&oid, _handle) == 0) {
        result = ObjectId(oid);
    }
    return result;
}
