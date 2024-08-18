#include "handle.h"

#include <repository.h>
#include <branch.h>

using namespace GIT;

AnnotatedCommitHandle AnnotatedCommitHandle::fromRef(Repository* repo, const Branch& branch)
{
    AnnotatedCommitHandle result;
    git_annotated_commit* handle = nullptr;
    ReferenceHandle referenceHandle = branch.reference().createHandle();
    if(referenceHandle.isNull() == false) {
        if(git_annotated_commit_from_ref(&handle, repo->handle().value(), referenceHandle.value()) == 0) {
            result = AnnotatedCommitHandle(handle);
        }
    }
    referenceHandle.dispose();
    return result;
}
