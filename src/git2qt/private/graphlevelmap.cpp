#include "graphlevelmap.h"
#include "log.h"

using namespace GIT;

const QString GraphLevelMap::MergeSourceTag     = "src-";

ObjectId::List GraphLevelMap::mergeIds() const
{
    ObjectId::List result;
    QStringList keys = GraphLevelMap::keys();
    for(const QString& key : keys) {
        if(key.startsWith(MergeSourceTag) && key.length() > MergeSourceTag.length() + 1) {
            ObjectId oid(key.mid(MergeSourceTag.length()));
            if(oid.isValid()) {
                result.append(oid);
            }
        }
        else {
            ObjectId oid(key);
            if(oid.isValid()) {
                result.append(oid);
            }
        }
    }
    return result;
}

void GraphLevelMap::addLevel(const QString &name, int level)
{
    insert(name, level);
}

void GraphLevelMap::removeLevel(const QString &level)
{
    remove(level);
}

void GraphLevelMap::removeLevel(int level)
{
    QStringList foundKeys;
    auto it = begin();
    for(;it != end();it++) {
        if(it.value() == level) {
            foundKeys.append(it.key());
        }
    }
    for(const QString& key : foundKeys) {
        remove(key);
    }
}

QString GraphLevelMap::makeMergeSourceString(const ObjectId &objectId)
{
    return QString("%1%2").arg(MergeSourceTag).arg(objectId.toString());
}
