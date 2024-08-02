#include "tagannotation.h"

using namespace GIT;

TagAnnotation::TagAnnotation(Repository* repo) :
    GitEntity(TagAnnotationEntity, repo)
{}
