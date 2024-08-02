#include "handle.h"

#include <repository.h>
#include <branch.h>

using namespace GIT;

AnnotatedCommitHandle AnnotatedCommitHandle::fromRef(Repository* repo, const Branch& branch)
{
    AnnotatedCommitHandle result;
    git_annotated_commit* handle = nullptr;
    if(git_annotated_commit_from_ref(&handle, repo->handle().value(), branch.reference().handle().value()) == 0) {
        result = AnnotatedCommitHandle(handle);
    }
    return result;
}
