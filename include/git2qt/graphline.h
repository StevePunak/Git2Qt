#ifndef GRAPHLINE_H
#define GRAPHLINE_H
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>

namespace GIT {

class GIT2QT_EXPORT GraphLine
{
public:
    GraphLine() {}

    QMap<int, GraphItemTypes> graphItems() const { return _graphItems; }
    void setGraphItem(int level, GraphItemTypes types)
    {
        _graphItems[level] |= types;
    }
    void makeHorizontals(int fromLevel, int toLevel);
    bool hasHorizontal(int atLevel) const;

    bool isValid() const { return _graphItems.count() != 0; }

    class List : public QList<GraphLine> {};
    class Map : public QMap<GIT::ObjectId, GraphLine> {};

private:
    QMap<int, GraphItemTypes> _graphItems;
};

} // namespace GIT

#endif // GRAPHLINE_H
