#include "lightweighttag.h"

using namespace GIT;

LightweightTag::LightweightTag(Repository* repo, const QString& name, const ObjectId& objectId) :
    Tag(repo, name, objectId)
{

}
