#ifndef HANDLE_H
#define HANDLE_H
#include <git2.h>

namespace GIT {

class Branch;

class Repository;

template <typename T>
class Handle
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

class ReferenceHandle : public Handle<git_reference*>
{
public:
    ReferenceHandle() : Handle() {}
    ReferenceHandle(git_reference* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_reference_free(_handle);
    }
};

class AnnotatedCommitHandle : public Handle<git_annotated_commit*>
{
public:
    AnnotatedCommitHandle() : Handle() {}
    AnnotatedCommitHandle(git_annotated_commit* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_annotated_commit_free(_handle);
    }

    static AnnotatedCommitHandle fromRef(Repository* repo, Branch* branch);
};

class ConfigurationHandle : public Handle<git_config*>
{
public:
    ConfigurationHandle() : Handle() {}
    ConfigurationHandle(git_config* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_config_free(_handle);
    }
};

class IndexHandle : public Handle<git_index*>
{
public:
    IndexHandle() : Handle() {}
    IndexHandle(git_index* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_index_free(_handle);
    }
};

class RemoteHandle : public Handle<git_remote*>
{
public:
    RemoteHandle() : Handle() {}
    RemoteHandle(git_remote* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_remote_free(_handle);
    }
};

class RepositoryHandle : public Handle<git_repository*>
{
public:
    RepositoryHandle() : Handle() {}
    RepositoryHandle(git_repository* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_repository_free(_handle);
    }
};

class TreeHandle : public Handle<git_tree*>
{
public:
    TreeHandle() : Handle() {}
    TreeHandle(git_tree* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_tree_free(_handle);
    }
};

class ObjectHandle : public Handle<git_object*>
{
public:
    ObjectHandle() : Handle() {}
    ObjectHandle(git_object* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_object_free(_handle);
    }
};

class SubmoudleHandle : public Handle<git_submodule*>
{
public:
    SubmoudleHandle() : Handle() {}
    SubmoudleHandle(git_submodule* handle) :
        Handle(handle) {}
    virtual void dispose() override
    {
        git_submodule_free(_handle);
    }
};

} // namespace GIT

#endif // HANDLE_H
