#include "annotatedtag.h"

using namespace GIT;

AnnotatedTag::AnnotatedTag(Repository* repo, const QString& name, const ObjectId& objectId) :
    Tag(repo, name, objectId)
{}
