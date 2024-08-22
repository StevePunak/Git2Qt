/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git remote.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REMOTE_H
#define REMOTE_H
#include <git2qt/gitentity.h>
#include <git2qt/handle.h>
#include <git2qt/referencecollection.h>
#include <QList>

namespace GIT {

class Repository;
class ReferenceCollection;
class Remote : public GitEntity
{
public:
    Remote();
    Remote(Repository* repo, const QString& name);
    virtual ~Remote();
    void dispose();

    QString name() const { return _name; }
    QString url() const { return _url; }

    Reference::List references() const { return _references.references(); }
    void reloadReferences();

    QString fetchSpecTransformToSource(const QString& value);

    RemoteHandle createHandle() const;

    virtual bool isNull() const override { return createHandle().isNull(); }

    class List : public QList<Remote>
    {
    public:
        Remote findByName(const QString& name) const
        {
            Remote result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const Remote& r) { return r.name() == name; } );
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    void commonInit();
    void reloadReferencesFromHeads();

    QString _name;
    QString _url;

    ReferenceCollection _references;
};

} // namespace GIT

#endif // REMOTE_H
