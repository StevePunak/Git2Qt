#ifndef INDEX_H
#define INDEX_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/indexentry.h>
#include <Kanoop/git/handle.h>
#include <Kanoop/git/compareoptions.h>

namespace GIT {

class Commit;
class Index : public QObject,
              public GitEntity

{
    Q_OBJECT
public:
    Index(Repository* repo);
    virtual ~Index();

    void remove(const QString& path);
    void add(const QString& path);
    void replace(const Commit& commit, const QStringList& paths, const CompareOptions& compareOptions = CompareOptions());
    void write();
    ObjectId writeTree();

    IndexEntry findByPath(const QString& path) { return _entries.findByPath(path); }
    IndexEntry::List entries() const { return _entries; }
    IndexHandle handle() const { return createHandle(); }

    virtual bool isNull() const override { return createHandle().isNull(); }

public slots:
    void reload();

private:
    IndexHandle createHandle() const;
    IndexEntry::List _entries;
};

} // namespace GIT

#endif // INDEX_H
