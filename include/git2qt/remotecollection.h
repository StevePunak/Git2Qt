/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of remotes. It is used by the repository
 * object to maintain the repo remotes.
 *
 * TODO: Should probably not be a public header.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REMOTECOLLECTION_H
#define REMOTECOLLECTION_H
#include <git2qt/gitentity.h>
#include <git2qt/remote.h>

namespace GIT {

class Repository;
class RemoteCollection : public GitEntity
{
public:
    RemoteCollection(Repository* repo);
    virtual ~RemoteCollection();

    void append(Remote* remote);
    Remote* value(int index) { return _remotes.at(index); }
    Remote* findByName(const QString& name) { return _remotes.findByName(name); }

    Remote::List remotes() const { return _remotes; }

    virtual bool isNull() const override { return false; }

private:
    Remote::List _remotes;
};

} // namespace GIT

#endif // REMOTECOLLECTION_H
