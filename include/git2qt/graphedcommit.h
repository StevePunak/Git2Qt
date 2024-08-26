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
    GraphedCommit(const Commit& other);

    int level() const { return _level; }
    void setLevel(int value) { _level = value; }

    QString branchName() const { return _branchName; }
    void setBranchName(const QString& value) { _branchName = value; }

    bool isHead() const { return _head; }
    void setHead(bool value) { _head = value; }

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

        int levelForThisParent(const Commit& commit)
        {
            int result = -1;
            auto it = std::find_if(begin(), end(), [commit](GraphedCommit& c)
            {
                return c.parents().count() > 0 && c.parents().at(0).objectId() == commit.objectId();
            });
            if(it != end()) {
                result = (*it).level();
            }
            return result;
        }

    };

private:
    int _level = 0;
    QString _branchName;
    bool _head = false;
    ObjectId::List _parentObjectIds;
};

} // namespace GIT

#endif // GRAPHEDCOMMIT_H
