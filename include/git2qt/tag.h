/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This abstract class represents a git tag. The concrete implementations
 * are AnnotatedTag and LightweightTag.
 *
 * This abstract class will be returned as a const Tag* from the repository
 * and should immediately be converted to the trivial concrete class via
 * toLightweightTag() and toAnnotatedTag().
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TAG_H
#define TAG_H
#include <git2qt/gitobject.h>
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>
#include <git2qt/handle.h>
#include <QList>

namespace GIT {

class AnnotatedTag;
class LightweightTag;
class Repository;
class Tag : public GitObject
{
public:
    Tag();
    Tag(GitEntityType entityType, Repository* repo, const QString& name, const ObjectId& objectId);
    Tag(const Tag& other);
    Tag& operator=(const Tag& other);
    virtual ~Tag();

    QString name() const { return _name; }
    QString shortName() const;

    GitObject* target() const { return _target; }

    LightweightTag toLightweightTag() const;
    AnnotatedTag toAnnotatedTag() const;

    bool isLightweight() const { return entityType() == TagLightweightEntity; }
    bool isAnnotated() const { return entityType() == TagAnnotatedEntity; }
    virtual bool isNull() const override { return _name.isEmpty(); }

    class PtrList : public QList<Tag*>
    {
    public:
        Tag* findByName(const QString& name) const
        {
            Tag* result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const Tag* t)
            {
                return t->name() == name || t->shortName() == name;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        Tag* findByObjectId(const ObjectId& objectId) const
        {
            Tag* result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Tag* t)
            {
                return t->objectId() == objectId;
            });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

    class ConstPtrList : public QList<const Tag*>
    {
    public:
        ConstPtrList() {}
        ConstPtrList(const PtrList& other)
        {
            for(Tag* tag : other) {
                append(tag);
            }
        }

        const Tag* findByName(const QString& name) const
        {
            const Tag* result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const Tag* t)
                                   {
                                       return t->name() == name || t->shortName() == name;
                                   });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

protected:
    void setName(const QString& value) { _name = value; }
    void setTarget(GitObject* value) { _target = value; }

private:
    QString _name;
    GitObject* _target = nullptr;
};

} // namespace GIT

#endif // TAG_H
