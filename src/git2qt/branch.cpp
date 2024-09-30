#include "branch.h"
#include "reference.h"
#include "commit.h"

#include "log.h"

#include <configuration.h>
#include <configurationentry.h>
#include <gitexception.h>
#include <network.h>
#include <remote.h>
#include <repository.h>
#include <utility.h>

using namespace GIT;

Branch::Branch(Repository* repo, const Reference& reference) :
    GitEntity(BranchEntity, repo),
    _reference(reference)
{
    ReferenceHandle refHandle = reference.createHandle();
    try
    {
        throwIfTrue(refHandle.isNull());
        _branchType = git_reference_is_remote(refHandle.value()) == 1 ? RemoteBranch : LocalBranch;
    }
    catch(const GitException&)
    {}
    refHandle.dispose();
}

Branch::Branch(const Branch& other) :
    GitEntity(BranchEntity, nullptr)
{
    *this = other;
}

Branch& Branch::operator=(const Branch& other)
{
    GitEntity::operator =(other);
    _reference = other._reference;
    _branchType = other._branchType;
    return *this;
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

QString Branch::friendlyName(bool trimOrigin) const
{
    QString result;
    if(_reference.looksLikeLocalBranch()) {
        result = _reference.canonicalName().mid(Reference::LocalBranchPrefix.length());
    }
    else if(_reference.looksLikeRemoteTrackingBranch()) {
        result = _reference.canonicalName().mid(Reference::RemoteTrackingBranchPrefix.length());
        QString remote = remoteName();
        if(trimOrigin && result.startsWith(remote) && result.length() > remote.length() + 1) {
            result = result.mid(remote.length() + 1);
        }
    }
    else {
        Log::logText(LVL_ERROR, QString("%1 does not look like a valid branch name").arg(_reference.canonicalName()));
    }
    return result;
}

QString Branch::upstreamBranchCanonicalName() const
{
    QString name;
    if(isRemote()) {
        Remote remote = repository()->network()->remoteForName(remoteName());
        name = remote.fetchSpecTransformToSource(_reference.canonicalName());
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

// "refs/heads/feature/test-branch:refs/remotes/origin/feature/feature/test-branch"

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

QString Branch::createRemoteName(const Remote& remote)
{
    QString result = Utility::combine(Reference::RemoteTrackingBranchPrefix, remote.name(), friendlyName());
    return result;
}

Commit Branch::tip()
{
    Commit commit(repository());

    try
    {
        ObjectId objid = _reference.targetObjectId();
        commit = Commit::lookup(repository(), objid);
        if(commit.isValid() == false) {
            throw GitException("Failed to find commit at starting reference");
        }
    }
    catch(const GitException&)
    {

    }

    return commit;
}

Commit Branch::birth()
{
    Commit commit(repository());

    try
    {
        Commit tipCommit = tip();
        Commit::List parents = tipCommit.parents();

    }
    catch(const GitException&)
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
