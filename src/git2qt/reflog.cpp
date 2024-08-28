#include "reflog.h"

#include <gitexception.h>
#include <repository.h>

using namespace GIT;

Reflog::Reflog(Repository* repo, const QString& canonicalName) :
    GitEntity(ReflogCollectionEntity, repo),
    _canonicalName(canonicalName)
{
    ReflogHandle handle = createHandle();
    try
    {
        throwIfTrue(handle.isNull());

        int count = git_reflog_entrycount(handle.value());
        for(int i = 0;i < count;i++) {
            const git_reflog_entry* entry = git_reflog_entry_byindex(handle.value(), i);
            throwIfNull(entry);
            _entries.append(ReflogEntry(entry));
        }
    }
    catch(const GitException&)
    {

    }
}

ReflogHandle Reflog::createHandle() const
{
    ReflogHandle result;
    git_reflog* reflog = nullptr;
    if(git_reflog_read(&reflog, repository()->handle().value(), _canonicalName.toUtf8().constData()) == 0) {
        result = ReflogHandle(reflog);
    }
    return result;

}
