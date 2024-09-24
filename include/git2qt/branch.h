/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents git branch.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef BRANCH_H
#define BRANCH_H
#include <git2qt/gitentity.h>
#include <git2qt/reference.h>

#include <QMap>

namespace GIT {

class Commit;
class Reference;
class Repository;
class Branch : public GitEntity
{
public:
    explicit Branch() : GitEntity(BranchEntity) {}
    explicit Branch(Repository* repo, const Reference& reference);
    Branch(const Branch& other);
    Branch& operator=(const Branch& other);

    virtual ~Branch() {}

    QString name() const;
    QString canonicalName() const;
    QString friendlyName(bool trimOrigin = false) const;
    QString upstreamBranchCanonicalName() const;
    QString upstreamBranchCanonicalNameFromLocalBranch() const;
    QString remoteName() const;
    BranchType branchType() const { return _branchType; }

    Reference reference() const { return _reference; }

    Commit tip();
    Commit birth();

    bool isHead() const;
    bool isRemote() const;
    bool isDetachedHead() const { return _detachedHead; }
    void setDetachedHead(bool value) { _detachedHead = value; }

    bool isValid() const { return canonicalName().isEmpty() == false; }
    virtual bool isNull() const override { return _reference.isNull(); }

    QVariant toVariant() const { return QVariant::fromValue<Branch>(*this); }
    static Branch fromVariant(const QVariant& value) { return value.value<Branch>(); }

    class List : public QList<Branch>
    {
    public:
        Branch findHead() const
        {
            Branch result;
            auto it = std::find_if(constBegin(), constEnd(), [](const Branch& b)
            {
                return b.isHead();
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        Branch findLocalBranch(const QString& branchName) const
        {
            Branch result;
            auto it = std::find_if(constBegin(), constEnd(), [branchName](const Branch& b)
            {
                return b.branchType() == LocalBranch && b.name() == branchName;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        Branch findRemoteBranch(const QString& branchName) const
        {
            Branch result;
            QString needle = QString("origin/%1").arg(branchName);
             auto it = std::find_if(constBegin(), constEnd(), [needle](const Branch& b)
            {
                return b.branchType() == RemoteBranch && b.name() == needle;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

    class Map : public QMap<QString, Branch>
    {
    public:
        Branch findForReferencedObjectId(const ObjectId& objectId) const
        {
            Branch branch;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Branch& branch)
            {
                return branch.reference().objectId() == objectId;
            });
            if(it != constEnd()) {
                branch = *it;
            }
            return branch;
        }
    };

private:
    QString remoteNameFromRemoteTrackingBranch() const;
    QString remoteNameFromLocalBranch() const;

    Reference _reference;
    BranchType _branchType = LocalBranch;
    bool _detachedHead = false;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::Branch)

#endif // BRANCH_H
