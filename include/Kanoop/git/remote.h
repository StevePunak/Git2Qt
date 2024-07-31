#ifndef REMOTE_H
#define REMOTE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/handle.h>
#include <QList>

namespace GIT {

class Repository;
class ReferenceCollection;
class Remote : public GitEntity
{
public:
    Remote(Repository* repo, const QString& name);
    virtual ~Remote();

    QString name() const { return _name; }
    QString url() const { return _url; }

    ReferenceCollection* references() const { return _references; }
    void reloadReferences();

    QString fetchSpecTransformToSource(const QString& value);

    RemoteHandle handle() const { return createHandle(); }

    virtual bool isNull() const override { return createHandle().isNull(); }

    class List : public QList<Remote*>
    {
    public:
        Remote* findByName(const QString& name) const
        {
            Remote* result = nullptr;
            auto it = std::find_if(constBegin(), constEnd(), [name](Remote* r) { return r->name() == name; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    void commonInit();
    RemoteHandle createHandle() const;

    QString _name;
    QString _url;

    ReferenceCollection* _references = nullptr;
};

} // namespace GIT

#endif // REMOTE_H
