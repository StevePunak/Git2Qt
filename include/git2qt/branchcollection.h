/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of git branches, and is used by the repository
 * object to maintain the collection.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef BRANCHCOLLECTION_H
#define BRANCHCOLLECTION_H
#include <git2qt/gitentity.h>
#include <git2qt/reference.h>
#include <git2qt/branch.h>

namespace GIT {

class Repository;
class BranchCollection : public GitEntity
{
public:
    BranchCollection(Repository* repo);

    Branch::List branches() const { return _cachedBranches; }
    Branch::List reloadBranches();
    void append(const Branch& branch) { _cachedBranches.append(branch); }

    Branch::Map localBranches() const;
    Branch::Map remoteBranches() const;

    Branch findLocalBranch(const QString& branchName) const { return _cachedBranches.findLocalBranch(branchName); }
    Branch findRemoteBranch(const QString& branchName) const { return _cachedBranches.findRemoteBranch(branchName); }

    Branch head() const { return _cachedBranches.findHead(); }

    virtual bool isNull() const  override;

private:
    Branch::List _cachedBranches;
};

} // namespace GIT

#endif // BRANCHCOLLECTION_H
