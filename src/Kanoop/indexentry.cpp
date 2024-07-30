#include "indexentry.h"

using namespace GIT;

IndexEntry::IndexEntry(git_index_entry* entry)
{
    _path = entry->path;
    _id = ObjectId(entry->id);
    _stageLevel = (StageLevel)git_index_entry_stage(entry);
    _mode = (Mode)entry->mode;
    _assumeUnchanged = (entry->flags & GIT_IDXENTRY_VALID) != 0;
}
