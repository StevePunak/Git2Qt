#ifndef BRANCH_H
#define BRANCH_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/reference.h>

#include <QMap>

namespace GIT {

class Commit;

class Reference;

class Repository;
class Branch : public GitEntity
{
public:
    enum BranchType
    {
        UnknownBranchType = 0,

        LocalBranch,
        RemoteBranch
    };

    explicit Branch(Repository* repo, const Reference& reference, git_branch_t type);
    virtual ~Branch();

    QString name() const;
    QString canonicalName() const;
    QString friendlyName() const;
    QString upstreamBranchCanonicalName();
    QString upstreamBranchCanonicalNameFromLocalBranch();
    QString remoteName();
    BranchType branchType() const { return _branchType; }

    Reference reference() const { return _reference; }

    Commit tip();

    bool isHead() const;
    bool isRemote() const;
    virtual bool isNull() const override { return _reference.isNull(); }

    class Map : public QMap<QString, Branch*>
    {
    public:
        Branch* findLocalBranch(const QString& branchName) const
        {
            Branch* result = nullptr;
            auto it = std::find_if(constBegin(), constEnd(), [branchName](Branch* b)
            {
                return b->branchType() == LocalBranch && b->name() == branchName;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        Branch* findRemoteBranch(const QString& branchName) const
        {
            Branch* result = nullptr;
            QString needle = QString("origin/%1").arg(branchName);
             auto it = std::find_if(constBegin(), constEnd(), [needle](Branch* b)
            {
                return b->branchType() == RemoteBranch && b->name() == needle;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    QString remoteNameFromRemoteTrackingBranch();
    QString remoteNameFromLocalBranch();

    Reference _reference;
    BranchType _branchType = LocalBranch;
};

} // namespace GIT

#endif // BRANCH_H
