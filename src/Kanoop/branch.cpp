#include "branch.h"
#include "reference.h"
#include "commit.h"

#include <Kanoop/commonexception.h>

using namespace GIT;

Branch::Branch(Repository* repo, Reference* reference, git_branch_t type) :
    GitEntity(GitEntity::BranchEntity, repo),
    _reference(reference)
{
    switch(type) {
    case GIT_BRANCH_LOCAL:
        _branchType = LocalBranch;
        break;
    case GIT_BRANCH_REMOTE:
        _branchType = RemoteBranch;
        break;
    default:
        break;
    }
}

Branch::~Branch()
{
    if(_reference != nullptr) {
        delete _reference;
    }
}

QString Branch::name() const
{
    const char* name;
    git_branch_name(&name, _reference->handle());
    return name;
}

Commit Branch::tip()
{
    Commit commit(repository());

    try
    {
        ObjectId objid = ObjectId::createFromReference(_reference);
        commit = Commit::lookup(repository(), objid);
        if(commit.isValid() == false) {
            throw CommonException("Failed to find commit at starting reference");
        }
    }
    catch(CommonException&)
    {

    }

    return commit;
}

bool Branch::isHead() const
{
    return git_branch_is_head(_reference->handle()) == 1 ? true : false;
}
