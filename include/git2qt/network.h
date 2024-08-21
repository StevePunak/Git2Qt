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

#include <QObject>

namespace GIT {

class CredentialResolver;
class Remote;
class RemoteCollection;
class Repository;
class Network : public QObject,
                public GitEntity
{
    Q_OBJECT
public:
    Network(Repository* repo);
    virtual ~Network();

    RemoteCollection* remotes();
    Remote* remoteForName(const QString& name) const;

    virtual bool isNull() const override { return false; }

public slots:
    void reload();

private:
    RemoteCollection* _remotes;
};

} // namespace GIT

#endif // NETWORK_H
