#ifndef NETWORK_H
#define NETWORK_H
#include <Kanoop/git/gitentity.h>

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
