#include "annotatedcommithandle.h"

#include <repository.h>

using namespace  GIT;

AnnotatedCommitHandle::AnnotatedCommitHandle(git_annotated_commit* handle) :
    _handle(handle) {}

AnnotatedCommitHandle::~AnnotatedCommitHandle()
{
    if(_handle != nullptr) {
        git_annotated_commit_free(_handle);
    }
}

AnnotatedCommitHandle* AnnotatedCommitHandle::fromRef(Repository* repo, Branch* branch)
{
    AnnotatedCommitHandle* result = nullptr;
    git_annotated_commit* handle = nullptr;
    if(git_annotated_commit_from_ref(&handle, repo->handle(), branch->reference()->handle()) == 0) {
        result = new AnnotatedCommitHandle(handle);
    }
    return result;
}
