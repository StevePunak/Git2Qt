#ifndef NETWORK_H
#define NETWORK_H
#include <Kanoop/git/gitentity.h>

namespace GIT {

class CredentialResolver;
class Remote;
class RemoteCollection;
class Repository;
class Network : public GitEntity
{
public:
    Network(Repository* repo);
    virtual ~Network();

    RemoteCollection* remotes();
    Remote* remoteForName(const QString& name) const;

    virtual bool isNull() const override { return false; }

private:
    RemoteCollection* _remotes;
};

} // namespace GIT

#endif // NETWORK_H
