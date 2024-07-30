#include "repositoryinformation.h"

#include <repository.h>

using namespace GIT;

RepositoryInformation::RepositoryInformation(Repository* repo) :
    GitEntity(RepositoryInfoEntity, repo)
{
    _isBare = git_repository_is_bare(repo->handle()) == 1;
    _path = git_repository_path(repo->handle());
    _workingDirectory = git_repository_workdir(repo->handle());
    _isShallow = git_repository_is_shallow(repo->handle()) == 1;
}

bool RepositoryInformation::isHeadDetached() const
{
    return git_repository_head_detached(repository()->handle()) == 1;
}

bool RepositoryInformation::isHeadUnborn() const
{
    return git_repository_head_unborn(repository()->handle()) == 1;
}

CurrentOperation::Op RepositoryInformation::currentOperation() const
{
    return (CurrentOperation::Op)git_repository_state(repository()->handle());
}

QString RepositoryInformation::message() const
{
    git_buf buf = GIT_BUF_INIT;
    git_repository_message(&buf, repository()->handle());
    return buf.ptr;
}
