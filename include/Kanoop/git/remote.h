#ifndef REMOTE_H
#define REMOTE_H
#include <Kanoop/git/gitentity.h>
#include <QList>

namespace GIT {

class Repository;
class ReferenceCollection;
class Remote : public GitEntity
{
public:
    Remote(Repository* repo, git_remote* handle);
    Remote(Repository* repo, const QString& name);
    virtual ~Remote();

    QString name() const { return _name; }
    QString url() const { return _url; }

    ReferenceCollection* references() const { return _references; }
    void reloadReferences();

    QString fetchSpecTransformToSource(const QString& value);

    git_remote* handle() const { return _handle; }

    virtual bool isNull() const override { return _handle == nullptr; }

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

    QString _name;
    QString _url;

    git_remote* _handle = nullptr;
    ReferenceCollection* _references = nullptr;
};

} // namespace GIT

#endif // REMOTE_H
