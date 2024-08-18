/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a concrete implemetation of an annotated tag.
 * As such, it is a trivial subclass of the abstract Tag class.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef ANNOTATEDTAG_H
#define ANNOTATEDTAG_H
#include <Kanoop/git/tag.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/signature.h>

namespace GIT {

class Repository;
class AnnotatedTag : public Tag
{
public:
    AnnotatedTag();
    AnnotatedTag(Repository* repo, const QString& name, const ObjectId& objectId);
    AnnotatedTag(const AnnotatedTag& other);
    AnnotatedTag& operator=(const AnnotatedTag& other);
    virtual ~AnnotatedTag();

    QString message() const { return _message; }
    Signature signature() const { return _signature; }

private:
    void commonInit();
    TagHandle createHandle() const;

    QString _message;
    Signature _signature;
};

} // namespace GIT

#endif // ANNOTATEDTAG_H
