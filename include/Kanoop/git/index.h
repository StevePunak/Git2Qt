#ifndef INDEX_H
#define INDEX_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/indexentry.h>
#include <Kanoop/git/handle.h>

namespace GIT {

class Index : public GitEntity
{
public:
    Index(Repository* repo);
    virtual ~Index();

    void remove(const QString& path);
    void add(const QString& path);
    void write();
    ObjectId writeTree();

    IndexEntry::List entries() const { return _entries; }
    IndexHandle handle() const { return createHandle(); }

    virtual bool isNull() const override { return createHandle().isNull(); }

private:
    IndexHandle createHandle() const;
    IndexEntry::List _entries;
};

} // namespace GIT

#endif // INDEX_H
