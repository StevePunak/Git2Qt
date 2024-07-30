#ifndef INDEX_H
#define INDEX_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/indexentry.h>

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
    git_index *handle() const { return _handle; }

    virtual bool isNull() const override { return _handle == nullptr; }

private:
    git_index *_handle;
    IndexEntry::List _entries;
};

} // namespace GIT

#endif // INDEX_H
