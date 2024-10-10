#include "graphline.h"

using namespace GIT;

void GraphLine::makeHorizontals(int fromLevel, int toLevel)
{
    if(fromLevel < toLevel) {
        if(hasHorizontal(fromLevel) == false) {
            setGraphItem(fromLevel, HorizontalRight);
        }
        if(hasHorizontal(toLevel) == false) {
            setGraphItem(toLevel, HorizontalLeft);
        }
        for(int level = fromLevel + 1;level < toLevel;level++) {
            setGraphItem(level, HorizontalLeft | HorizontalRight);
        }
    }
    else if(fromLevel > toLevel) {
        if(hasHorizontal(fromLevel) == false) {
            setGraphItem(fromLevel, HorizontalLeft);
        }
        if(hasHorizontal(toLevel) == false) {
            setGraphItem(toLevel, HorizontalRight);
        }
        for(int level = toLevel + 1;level < fromLevel;level++) {
            setGraphItem(level, HorizontalLeft | HorizontalRight);
        }
    }
    else {

    }
}

bool GraphLine::hasHorizontal(int atLevel) const
{
    return _graphItems.contains(atLevel) && (_graphItems.value(atLevel) & AnyHorizontal) != 0;
}
