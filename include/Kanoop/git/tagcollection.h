/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of Tag objects and is used
 * by the repository to maintain its submodule information.
 *
 * TODO: Should probably not be a public header.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef TAGCOLLECTION_H
#define TAGCOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/tag.h>

namespace GIT {

class LightweightTag;

class AnnotatedTag;

class Commit;
class Repository;
class Signature;
class TagCollection : public QObject,
                      public GitEntity
{
    Q_OBJECT
public:
    TagCollection(Repository* repo);

    const LightweightTag* createLightweightTag(const QString& name, const GitObject& targetObject);
    const AnnotatedTag* createAnnotatedTag(const QString& name, const QString& message, const Signature& signature, const GitObject& targetObject);
    const Tag* findTag(const QString& name) const;
    bool deleteLocalTag(const QString& name);

    Tag::ConstPtrList tags() const { return _tags; }

    virtual bool isNull() const override;

public slots:
    void reload();

private:
    void retrieveTags();

    // Callbacks
    static int gitTagForeachCallback(const char *name, git_oid *oid, void *payload);

    Tag::PtrList _tags;
};

} // namespace GIT

#endif // TAGCOLLECTION_H
