#ifndef TAGANNOTATION_H
#define TAGANNOTATION_H
#include <Kanoop/git/gitobject.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/signature.h>

namespace GIT {

class Repository;
class TagAnnotation : public GitObject
{
public:
    TagAnnotation();
    TagAnnotation(Repository* repo, const ObjectId& objectId);
    TagAnnotation(const TagAnnotation& other);
    TagAnnotation& operator=(const TagAnnotation& other);
    virtual ~TagAnnotation();

    QString name() const { return _name; }
    QString message() const { return _message; }
    Signature signature() const { return _signature; }

    GitObject* target() const { return _target; }
    QString shortName() const;

    virtual bool isNull() const override { return false; }

    class List : public QList<TagAnnotation>
    {
    public:
        TagAnnotation findByName(const QString& name) const
        {
            TagAnnotation result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const TagAnnotation& t)
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
    TagHandle createHandle() const;

    QString _name;
    QString _message;
    Signature _signature;
    GitObject* _target = nullptr;
};

} // namespace GIT

#endif // TAGANNOTATION_H
