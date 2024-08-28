#include "reflogentry.h"

using namespace GIT;

ReflogEntry::ReflogEntry(const git_reflog_entry* entry)
{
    _from = git_reflog_entry_id_old(entry);
    _to = git_reflog_entry_id_new(entry);
    _committer = git_reflog_entry_committer(entry);
    _message = git_reflog_entry_message(entry);
}
