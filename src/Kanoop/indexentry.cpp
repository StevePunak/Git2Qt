#include "indexentry.h"

using namespace GIT;

IndexEntry::IndexEntry(const QString& path, Mode mode, StageLevel stageLevel, bool assumeUnchanged, const ObjectId& objectId) :
    _path(path), _mode(mode), _stageLevel(stageLevel), _assumeUnchanged(assumeUnchanged), _objectId(objectId)
{
    // _path = path->path;
    // _objectId = ObjectId(path->id);
    // _stageLevel = (StageLevel)git_index_entry_stage(path);
    // _mode = (Mode)path->mode;
    // _assumeUnchanged = (path->flags & GIT_IDXENTRY_VALID) != 0;
}
