#ifndef REMOTECOLLECTION_H
#define REMOTECOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/remote.h>

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

    virtual bool isNull() const override { return false; }

private:
    Remote::List _remotes;
};

} // namespace GIT

#endif // REMOTECOLLECTION_H
