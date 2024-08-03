#include "branch.h"
#include "reference.h"
#include "commit.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>

#include <configuration.h>
#include <configurationentry.h>
#include <network.h>
#include <remote.h>
#include <repository.h>

using namespace GIT;

Branch::Branch(Repository* repo, const Reference& reference, git_branch_t type) :
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
}

QString Branch::name() const
{
    QString name;
    const char* namePtr;
    ReferenceHandle handle = _reference.createHandle();
    if(handle.isNull() == false) {
        if(git_branch_name(&namePtr, handle.value()) == 0) {
            name = namePtr;
        }
    }
    handle.dispose();
    return name;
}

QString Branch::canonicalName() const
{
    return _reference.canonicalName();
}

QString Branch::friendlyName() const
{
    QString result;
    if(_reference.looksLikeLocalBranch()) {
        result = _reference.canonicalName().mid(Reference::LocalBranchPrefix.length());
    }
    else if(_reference.looksLikeRemoteTrackingBranch()) {
        result = _reference.canonicalName().mid(Reference::RemoteTrackingBranchPrefix.length());
    }
    else {
        KLog::sysLogText(KLOG_ERROR, QString("%1 does not look like a valid branch name").arg(_reference.canonicalName()));
    }
    return result;
}

QString Branch::upstreamBranchCanonicalName() const
{
    QString name;
    if(isRemote()) {
        Remote* remote = repository()->network()->remoteForName(remoteName());
        name = remote->fetchSpecTransformToSource(_reference.canonicalName());
    }
    else {
        name = upstreamBranchCanonicalNameFromLocalBranch();
    }
    return name;
}

QString Branch::upstreamBranchCanonicalNameFromLocalBranch() const
{
    QString result;
    ConfigurationEntry entry = repository()->config()->get("branch", friendlyName(), "merge");
    if(entry.isValid()) {
        result = entry.value().toString();
    }
    return result;
}

QString Branch::remoteName() const
{
    QString result;
    if(isRemote()) {
        result = remoteNameFromRemoteTrackingBranch();
    }
    else {
        result = remoteNameFromLocalBranch();
    }
    return result;
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
    bool result = false;
    ReferenceHandle handle = _reference.createHandle();
    if(handle.isNull() == false) {
        result = git_branch_is_head(handle.value()) == 1 ? true : false;
    }
    handle.dispose();
    return result;
}

bool Branch::isRemote() const
{
    return _reference.looksLikeRemoteTrackingBranch();
}

QString Branch::remoteNameFromRemoteTrackingBranch() const
{
    QString result;
    git_buf buf = GIT_BUF_INIT;
    if(git_branch_remote_name(&buf, repository()->handle().value(), _reference.canonicalName().toUtf8().constData()) == 0) {
        result = buf.ptr;
    }
    return result;
}

QString Branch::remoteNameFromLocalBranch() const
{
    QString result;
    ConfigurationEntry entry = repository()->config()->get("branch", friendlyName(), "remote");
    if(entry.isValid()) {
        result = entry.value().toString();
        if(result.isEmpty() || result == ".") {
            result = QString();
        }
    }
    return result;
}
