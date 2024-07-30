#ifndef INDEXENTRY_H
#define INDEXENTRY_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>

#include <QList>

namespace GIT {

class IndexEntry
{
public:
    IndexEntry(git_index_entry* entry);

    class List : public QList<IndexEntry>
    {
    };

private:
    QString _path;
    Mode _mode;
    StageLevel _stageLevel;
    bool _assumeUnchanged;
    ObjectId _id;
};

} // namespace GIT

#endif // INDEXENTRY_H
