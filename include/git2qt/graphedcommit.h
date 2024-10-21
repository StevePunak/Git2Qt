/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Subclass of Commit to provide graphing properties.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GRAPHEDCOMMIT_H
#define GRAPHEDCOMMIT_H
#include <git2qt/commit.h>
#include <git2qt/graphline.h>

namespace GIT {

class GIT2QT_EXPORT GraphedCommit : public Commit
{
public:
    GraphedCommit();
    GraphedCommit(const Commit& other);

    bool operator ==(const GraphedCommit& other) const;
    bool operator !=(const GraphedCommit& other) const { return !(*this == other); }

    int level() const { return _level; }
    void setLevel(int value);

    int maxLevel() const { return _maxLevel; }
    void setMaxLevel(int value) { _maxLevel = value; }

    int index() const { return _index; }
    void setIndex(int value) { _index = value; }

    QString friendlyBranchName() const { return _friendlyBranchName; }
    void setFriendlyBranchName(const QString& value) { _friendlyBranchName = value; }

    QString branchName() const { return _branchName; }
    void setBranchName(const QString& value) { _branchName = value; }

    bool isHead() const { return _head; }
    void setHead(bool value) { _head = value; }

    bool isMerge() const { return _merge; }
    void setMerge(bool value) { _merge = value; }

    bool isStash() const { return _stash; }
    void setStash(bool value) { _stash = value; }

    bool isStashBase() const { return _stashBaseOf.isValid(); }

    bool isStashParent() const { return _stashParent; }
    void setStashParent(bool value) { _stashParent = value; }

    bool isRemote() const { return _remote; }
    void setRemote(bool value) { _remote = value; }

    ObjectId mergeBase() const { return _mergeBase; }
    void setMergeBase(const ObjectId& value) { _mergeBase = value; }

    ObjectId mergeFrom() const { return _mergeFrom; }
    void setMergeFrom(const ObjectId& value) { _mergeFrom = value; }

    ObjectId mergedInto() const { return _mergedInto; }
    void setMergedInto(const ObjectId& value) { _mergedInto = value; }

    ObjectId stashBaseOf() const { return _stashBaseOf; }
    void setStashBaseOf(const ObjectId& value) { _stashBaseOf = value; }

    QStringList branchBases() const { return _branchBases; }
    void setBranchBases(const QStringList& value) { _branchBases = value; }
    void addBranchBase(const QString& value) { if(_branchBases.contains(value) == false) _branchBases.append(value); }

    int distanceAhead(const Commit& other) const;
    int distanceBehind(const Commit& other) const;

    ObjectId::List parentObjectIds() const { return _parentObjectIds; }
    void setParentObjectIds(const ObjectId::List& value) { _parentObjectIds = value; }

    ObjectId::List childObjectIds() const { return _childObjectIds; }
    void setChildObjectIds(const ObjectId::List& value) { _childObjectIds = value; }

    GraphLine graphLine() const { return _graphLine; }
    GraphLine& graphLineRef() { return _graphLine; }
    void setGraphLine(const GraphLine& value) { _graphLine = value; }

    QVariant toVariant() const { return QVariant::fromValue<GraphedCommit>(*this); }
    static GraphedCommit fromVariant(const QVariant& value) { return value.value<GraphedCommit>(); }

    QString toString() const;

    class List : public QList<GraphedCommit>
    {
    public:
        List() {}
        List(const Commit::List& other) {
            for(const Commit& commit : other) {
                append(GraphedCommit(commit));
            }
        }

        List reverse() const
        {
            List result = *this;
            std::reverse(result.begin(), result.end());
            return result;
        }

        Commit::List toCommitList() const
        {
            Commit::List result;
            for(const GraphedCommit& commit : *this) {
                result.append(commit);
            }
            return result;
        }

        GraphedCommit findCommit(const ObjectId& objectId) const
        {
            GraphedCommit result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const GraphedCommit& c) { return c.objectId() == objectId; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        GraphedCommit* getCommit(const ObjectId& objectId)
        {
            GraphedCommit* result = nullptr;
            auto it = std::find_if(begin(), end(), [objectId](const GraphedCommit& c) { return c.objectId() == objectId; } );
            if(it != end()) {
                result = &(*it);
            }
            return result;
        }

        List findChildren(const GraphedCommit& of) const;

        bool hasParent(const ObjectId& parentObjectId)
        {
            auto it = std::find_if(begin(), end(), [parentObjectId](GraphedCommit& c)
            {
                return c.parents().objectIds().contains(parentObjectId);
            });
            bool result = it != end();
            return result;
        }

        bool containsObjectId(const ObjectId& objectId) const
        {
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const GraphedCommit& c)
            {
                return c.objectId() == objectId;
            });
            bool result = it != constEnd();
            return result;
        }

        int levelForBranchName(const QString& branchName) const
        {
            int result = -1;
            auto it = std::find_if(constBegin(), constEnd(), [branchName](const GraphedCommit& c)
            {
                return c.friendlyBranchName() == branchName;
            });
            if(it != constEnd()) {
                result = (*it).level();
            }
            return result;
        }

        int levelForCommit(const ObjectId& commitId) const
        {
            return findCommit(commitId).level();
        }

        int maxLevel() const
        {
           int level = 1;
           auto it = std::max_element(constBegin(), constEnd(), [](const GraphedCommit& a, const GraphedCommit& b) { return a.level() < b.level(); });
           if(it != constEnd()) {
               level = (*it).level();
           }
           return level;
        }

    };

    class Map : public QMap<ObjectId, GraphedCommit>
    {
    public:
        Map() {}
        Map(const GraphedCommit::List& other) {
            for(const GraphedCommit& commit : other) {
                insert(commit.objectId(), commit);
            }
        }

        GraphedCommit::List findCommits(const ObjectId::List& objectIds) const
        {
            GraphedCommit::List result;
            for(const ObjectId& objectId : objectIds) {
                GraphedCommit commit = value(objectId);
                if(commit.isValid()) {
                    result.append(commit);
                }
            }
            return result;
        }
    };

private:
    static bool getDistance(Repository* repo, const ObjectId& local, const ObjectId& upstream, size_t* ahead, size_t* behind);

    int _level = 0;
    int _maxLevel = 0;
    int _index = 0;
    QString _friendlyBranchName;
    QString _branchName;
    bool _head = false;
    bool _merge = false;
    bool _stash = false;
    bool _stashParent = false;
    bool _remote = false;
    ObjectId _mergeBase;
    ObjectId _mergeFrom;
    ObjectId _mergedInto;
    ObjectId _stashBaseOf;
    QStringList _branchBases;
    ObjectId::List _parentObjectIds;
    ObjectId::List _childObjectIds;
    GraphLine _graphLine;
};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::GraphedCommit)

#endif // GRAPHEDCOMMIT_H
