#include "commitfilter.h"

using namespace GIT;

CommitFilter::CommitFilter() :
    _sortBy(SortStrategyTime),
    _includeReachableFromStr("HEAD"),
    _firstParentOnly(false)
{
}
