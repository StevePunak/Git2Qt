#ifndef GRAPHLINEITEM_H
#define GRAPHLINEITEM_H
#include <git2qt/graphline.h>

namespace GIT {

class GIT2QT_EXPORT GraphLineItem
{
public:
    GraphLineItem() :
        _graphTypes(NoGraphItem) {}

    GraphItemTypes graphTypes() const { return _graphTypes; }
    void setGraphTypes(GraphItemTypes value) { _graphTypes = value; }
    void addGraphTypes(GraphItemTypes value) { _graphTypes |= value; }

    bool isValid() const { return _graphTypes != NoGraphItem; }

    class Map : public QMap<int, GraphLineItem> {};

private:
    GraphItemTypes _graphTypes;
};

} // namespace GIT

#endif // GRAPHLINEITEM_H
