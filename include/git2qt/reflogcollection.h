/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of ReflogEntries. It is used by the repository
 * object to maintain the repo references.
 *
 * TODO: Should probably not be a public header.
 *
 * Stephen Punak, August 21, 2024
*/
#ifndef REFLOGCOLLECTION_H
#define REFLOGCOLLECTION_H
#include <git2qt/gitentity.h>
#include <git2qt/objectid.h>
#include <git2qt/reflogentry.h>
#include <git2qt/handle.h>

namespace GIT {

class Repository;

class ReflogCollection : public GitEntity
{
public:
    ReflogCollection(Repository* repo, const QString& canonicalName);

    QString canonicalName() const { return _canonicalName; }
    ReflogEntry::List entries() const { return _entries; }

    virtual bool isNull() const override { return _canonicalName.isEmpty(); }

private:
    ReflogHandle createHandle() const;

    QString _canonicalName;
    ReflogEntry::List _entries;
};

} // namespace GIT

#endif // REFLOGCOLLECTION_H
