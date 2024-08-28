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

namespace GIT {

class GraphedCommit : public Commit
{
public:
    GraphedCommit();
    GraphedCommit(const Commit& other);

    bool operator ==(const GraphedCommit& other) const;
    bool operator !=(const GraphedCommit& other) const { return !(*this == other); }

    int level() const { return _level; }
    void setLevel(int value) { _level = value; }

    QString branchName() const { return _branchName; }
    void setBranchName(const QString& value) { _branchName = value; }

    bool isHead() const { return _head; }
    void setHead(bool value) { _head = value; }

    bool isMerge() const { return _merge; }
    void setMerge(bool value) { _merge = value; }

    ObjectId mergeBase() const { return _mergeBase; }
    void setMergeBase(const ObjectId& value) { _mergeBase = value; }

    ObjectId mergeFrom() const { return _mergeFrom; }
    void setMergeFrom(const ObjectId& value) { _mergeFrom = value; }

    ObjectId mergedInto() const { return _mergedInto; }
    void setMergedInto(const ObjectId& value) { _mergedInto = value; }

    int distanceAhead(const Commit& other) const;
    int distanceBehind(const Commit& other) const;

    ObjectId::List parentObjectIds() const { return _parentObjectIds; }

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

        Commit::List toCommitList() const
        {
            Commit::List result;
            for(const GraphedCommit& commit : *this) {
                result.append(commit);
            }
            return result;
        }

        int distanceBetween(const GraphedCommit& a, const GraphedCommit& b) const
        {
            int result = -1;
            int indexA = indexOf(a);
            int indexB = indexOf(b);
            result = std::abs(indexA - indexB);
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

        List findChildren(const GraphedCommit& of)
        {
            List result;
            for(GraphedCommit& commit : *this) {
                if(commit.parents().objectIds().contains(of.objectId())) {
                    result.append(commit);
                }
            }
            return result;
        }

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
                return c.branchName() == branchName;
            });
            if(it != constEnd()) {
                result = (*it).level();
            }
            return result;
        }

        int levelForChild(const GraphedCommit& commit) const
        {
            int result = -1;
#if 0
            // look backwards through this collection for the best match
            for(int i = count() - 1;i >= 0;i--) {
                const GraphedCommit& candidate = at(i);
                Commit::List parents = candidate.parents();
                if(parents.count() == 1 && parents.at(0).objectId() == commit.objectId()) {
                    result = candidate.level();
                    break;
                }
                if(parents.count() > 1) {
                    // TODO - handle merge
                    if(parents.at(0).objectId() == commit.objectId()) {
                        result = candidate.level();
                        break;
                    }
                }
            }
#else
            // Build a list of candidates
            List candidates;
            for(int i = count() - 1;i >= 0;i--) {
                const GraphedCommit& candidate = at(i);
                Commit::List parents = candidate.parents();
                for(const Commit& parent : parents) {
                    if(parent.objectId() == commit.objectId()) {
                        candidates.append(candidate);
                        break;
                    }
                }
            }

            if(candidates.count() == 1) {
                GraphedCommit candidate = candidates.at(0);
                if(candidate.isMerge() == false) {
                    result = candidates.at(0).level();
                }
            }
            else if(candidates.count() == 2) {
                // this is a merge. get the best branch
                for(const GraphedCommit& candidate : candidates) {
                    int ahead = commit.distanceAhead(candidate);    Q_UNUSED(ahead)
                    int behind = commit.distanceBehind(candidate);    Q_UNUSED(behind)
                    if(candidate.branchName() == commit.branchName()) {
                        result = candidate.level();
                        break;
                    }
                }
            }
            else {
                int x = 1; Q_UNUSED(x)
            }
#endif
            return result;
        }

    };

private:
    static bool getDistance(Repository* repo, const ObjectId& local, const ObjectId& upstream, size_t* ahead, size_t* behind);

    int _level = 0;
    QString _branchName;
    bool _head = false;
    bool _merge = false;
    ObjectId::List _parentObjectIds;
    ObjectId _mergeBase;
    ObjectId _mergeFrom;
    ObjectId _mergedInto;
};

} // namespace GIT

#endif // GRAPHEDCOMMIT_H
