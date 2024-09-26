/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Handle wrappers for git_... handles from libgit2.
 *
 * Note that the handles are not freed when the object is destroyed.
 * This allows the handles to be copied and are valid until dispose() is
 * called.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef HANDLE_H
#define HANDLE_H
#include <git2.h>
#include <git2qt/declspec.h>

namespace GIT {

class Branch;

class Repository;

template <typename T>
class GIT2QT_EXPORT Handle
{
public:
    Handle() :
        _handle(nullptr) {}
    Handle(T handle) :
        _handle(handle) {}
    virtual ~Handle() {}

    virtual void dispose() = 0;

    T value() const { return _handle; }


    bool isNull() const { return _handle == nullptr; }

protected:
    T _handle;
};

class GIT2QT_EXPORT ReferenceHandle : public Handle<git_reference*>
{
public:
    ReferenceHandle() : Handle() {}
    ReferenceHandle(git_reference* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_reference_free(_handle);
        }
    }
};

class GIT2QT_EXPORT AnnotatedCommitHandle : public Handle<git_annotated_commit*>
{
public:
    AnnotatedCommitHandle() : Handle() {}
    AnnotatedCommitHandle(git_annotated_commit* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_annotated_commit_free(_handle);
        }
    }

    static AnnotatedCommitHandle fromRef(Repository* repo, const Branch& branch);
};

class GIT2QT_EXPORT CommitHandle : public Handle<git_commit*>
{
public:
    CommitHandle() : Handle() {}
    CommitHandle(git_commit* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_commit_free(_handle);
        }
    }

    static AnnotatedCommitHandle fromRef(Repository* repo, const Branch& branch);
};

class GIT2QT_EXPORT ConfigurationHandle : public Handle<git_config*>
{
public:
    ConfigurationHandle() : Handle() {}
    ConfigurationHandle(git_config* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_config_free(_handle);
        }
    }
};

class GIT2QT_EXPORT IndexHandle : public Handle<git_index*>
{
public:
    IndexHandle() : Handle() {}
    IndexHandle(git_index* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_index_free(_handle);
        }
    }
};

class GIT2QT_EXPORT RemoteHandle : public Handle<git_remote*>
{
public:
    RemoteHandle() : Handle() {}
    RemoteHandle(git_remote* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_remote_free(_handle);
        }
    }
};

class GIT2QT_EXPORT RepositoryHandle : public Handle<git_repository*>
{
public:
    RepositoryHandle() : Handle() {}
    RepositoryHandle(git_repository* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_repository_free(_handle);
        }
    }
};

class GIT2QT_EXPORT TreeHandle : public Handle<git_tree*>
{
public:
    TreeHandle() : Handle() {}
    TreeHandle(git_tree* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_tree_free(_handle);
        }
    }
};

class GIT2QT_EXPORT ObjectHandle : public Handle<git_object*>
{
public:
    ObjectHandle() : Handle() {}
    ObjectHandle(git_object* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_object_free(_handle);
        }
    }
};

class GIT2QT_EXPORT SubmoudleHandle : public Handle<git_submodule*>
{
public:
    SubmoudleHandle() : Handle() {}
    SubmoudleHandle(git_submodule* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_submodule_free(_handle);
        }
    }
};

class GIT2QT_EXPORT TagHandle : public Handle<git_tag*>
{
public:
    TagHandle() : Handle() {}
    TagHandle(git_tag* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_tag_free(_handle);
        }
    }
};

class GIT2QT_EXPORT DiffHandle : public Handle<git_diff*>
{
public:
    DiffHandle() : Handle() {}
    DiffHandle(git_diff* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_diff_free(_handle);
        }
    }
};

class GIT2QT_EXPORT RevWalkHandle : public Handle<git_revwalk*>
{
public:
    RevWalkHandle() : Handle() {}
    RevWalkHandle(git_revwalk* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_revwalk_free(_handle);
        }
    }
};

class GIT2QT_EXPORT ReflogHandle : public Handle<git_reflog*>
{
public:
    ReflogHandle() : Handle() {}
    ReflogHandle(git_reflog* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_reflog_free(_handle);
        }
    }
};

class GIT2QT_EXPORT BlobHandle : public Handle<git_blob*>
{
public:
    BlobHandle() : Handle() {}
    BlobHandle(git_blob* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_blob_free(_handle);
        }
    }
};

class GIT2QT_EXPORT ObjectDatabaseHandle : public Handle<git_odb*>
{
public:
    ObjectDatabaseHandle() : Handle() {}
    ObjectDatabaseHandle(git_odb* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        if(_handle != nullptr) {
            git_odb_free(_handle);
        }
    }
};

} // namespace GIT

#endif // HANDLE_H
