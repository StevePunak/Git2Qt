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

    Reference::List includeReachableFromRefs() const { return _includeReachableFromRefs; }
    void setIncludeReachableFrom(const Reference::List& value) { _includeReachableFromRefs = value; }

    Reference::List excludeReachableFromRefs() const { return _excludeReachableFromRefs; }
    void setExcludeReachableFromRefs(const Reference::List& value) { _excludeReachableFromRefs = value; }

    CommitSortStrategies sortBy() const { return _sortBy; }
    void setSortBy(CommitSortStrategies value) { _sortBy = value; }

    bool firstParentOnly() const { return _firstParentOnly; }
    void setFirstParentOnly(bool value) { _firstParentOnly = value; }

private:
    CommitSortStrategies _sortBy;
    QString _includeReachableFromStr;
    Reference::List _includeReachableFromRefs;
    Reference::List _excludeReachableFromRefs;
    bool _firstParentOnly;
};


// STOP WALK IN GOOLE FIT


} // namespace GIT

#endif // COMMITFILTER_H
