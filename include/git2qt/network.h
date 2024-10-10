/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is a port of the Network class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/

#ifndef NETWORK_H
#define NETWORK_H
#include <git2qt/gitentity.h>
#include <git2qt/remote.h>
#include <git2qt/fetchhead.h>

#include <QObject>

namespace GIT {

class AbstractCredentialResolver;
class RemoteCollection;
class Repository;
class GIT2QT_EXPORT Network : public QObject,
                              public GitEntity
{
    Q_OBJECT
public:
    Network(Repository* repo);
    virtual ~Network();

    Remote::List remotes();
    Remote remoteForName(const QString& name) const;

    FetchHead::List fetchHeads();

    virtual bool isNull() const override { return false; }

public slots:
    void reload();

private:
    RemoteCollection* _remotes;

    static int fetchHeadCallback(const char *ref_name, const char *remote_url, const git_oid *oid, unsigned int is_merge, void *payload);
};

} // namespace GIT

#endif // NETWORK_H
