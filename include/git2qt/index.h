/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Represents a git_index from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef INDEX_H
#define INDEX_H
#include <git2qt/gitentity.h>
#include <git2qt/indexentry.h>
#include <git2qt/handle.h>
#include <git2qt/compareoptions.h>

namespace GIT {

class TreeChanges;

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
    void add(const QString& path, const ObjectId& objectId, Mode mode);
    void replace(const Commit& commit, const QStringList& paths, const CompareOptions& compareOptions = CompareOptions());
    void replace(const TreeChanges& changes);
    void write();
    ObjectId writeTree();

    IndexHandle createHandle() const;

    IndexEntry findByPath(const QString& path) { return _entries.findByPath(path); }
    IndexEntry::List entries() const { return _entries; }
    virtual bool isNull() const override;

public slots:
    void reload();

private:
    IndexEntry::List _entries;
};

} // namespace GIT

#endif // INDEX_H
