#ifndef COMMITFILTER_H
#define COMMITFILTER_H
#include <git2qt/gittypes.h>
#include <git2qt/reference.h>

namespace GIT {

class Repository;
class CommitFilter
{
public:
    CommitFilter();

    void setIncludeReachableFrom(const QString value) { _includeReachableFromStr = value; }

    ObjectId::List includeReachableFromRefs() const { return _includeReachableFromRefs; }
    void setIncludeReachableFrom(const Reference::List& value) { _includeReachableFromRefs.append(value.objectIds()); }
    void setIncludeReachableFrom(const Reference& value) { _includeReachableFromRefs.append(value.objectId()); }
    void setIncludeReachableFrom(const ObjectId& value) { _includeReachableFromRefs.append(value); }
    void setIncludeReachableFrom(const ObjectId::List& value) { _includeReachableFromRefs.append(value); }

    ObjectId::List excludeReachableFromRefs() const { return _excludeReachableFromRefs; }
    void setExcludeReachableFromRefs(const ObjectId::List& value) { _excludeReachableFromRefs.append(value); }
    void setExcludeReachableFromRefs(const ObjectId& value) { _excludeReachableFromRefs.append(value); }

    CommitSortStrategies sortBy() const { return _sortBy; }
    void setSortBy(CommitSortStrategies value) { _sortBy = value; }

    bool firstParentOnly() const { return _firstParentOnly; }
    void setFirstParentOnly(bool value) { _firstParentOnly = value; }

    int maxResults() const { return _maxResults; }
    void setMaxResults(int value) { _maxResults = value; }

private:
    CommitSortStrategies _sortBy;
    QString _includeReachableFromStr;
    ObjectId::List _includeReachableFromRefs;
    ObjectId::List _excludeReachableFromRefs;
    bool _firstParentOnly = false;
    int _maxResults = 0;
};


// STOP WALK IN GOOLE FIT


} // namespace GIT

#endif // COMMITFILTER_H
