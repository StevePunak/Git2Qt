#ifndef TAG_H
#define TAG_H
#include <Kanoop/git/gitobject.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/handle.h>
#include <QList>

namespace GIT {

class Repository;
class Tag : public GitObject
{
public:
    Tag();
    Tag(Repository* repo, const QString& name, const ObjectId& objectId);
    Tag(const Tag& other);
    Tag& operator=(const Tag& other);
    virtual ~Tag();

    QString name() const { return _name; }
    QString shortName() const;

    GitObject* target() const { return _target; }

    virtual bool isNull() const override { return _name.isEmpty(); }

    class List : public QList<Tag>
    {
    public:
        Tag findByName(const QString& name) const
        {
            Tag result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const Tag& t)
            {
                return t.name() == name || t.shortName() == name;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

private:
    void commonInit();

    QString _name;
    GitObject* _target = nullptr;
};

} // namespace GIT

#endif // TAG_H
