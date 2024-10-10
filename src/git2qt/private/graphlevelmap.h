#ifndef GRAPHLEVELMAP_H
#define GRAPHLEVELMAP_H
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>

namespace GIT {

class GraphLevelMap : private QMap<QString, int>
{
public:
    QList<int> levelsInUse() const
    {
        return values();
    }

    ObjectId::List mergeIds() const;

    void addLevel(const QString& name, int level);

    void removeLevel(const QString& level);

    void removeLevel(int level);

    QString nameForLevel(int level) const
    {
        QString name;
        for(auto it = constBegin();it != constEnd();it++) {
            if(it.value() == level) {
                name = it.key();
                break;
            }
        }
        return name;
    }

    bool containsName(const QString& name) const
    {
        return contains(name);
    }

    bool containsLevel(int level) const
    {
        auto it = std::find_if(constBegin(), constEnd(), [level](int value) { return level == value; });
        return it != constEnd();
    }

    int levelForName(const QString& name) const
    {
        return value(name, 0);
    }

    int availableLevel(int exclude = 0) const
    {
        int result = 1;
        for(;result == exclude || levelsInUse().contains(result) == true;result++);
        return result;
    }

    int maxLevel() const
    {
        int level = 0;
        QList<int> levels = values();
        auto it = std::max_element(levels.constBegin(), levels.constEnd());
        if(it != levels.constEnd()) {
            level = *it;
        }
        return level;
    }

    static QString makeMergeSourceString(const ObjectId& objectId);

    static const QString MergeSourceTag;
};

} // namespace GIT

#endif // GRAPHLEVELMAP_H
