#ifndef FETCHHEAD_H
#define FETCHHEAD_H
#include <git2qt/gitentity.h>
#include <git2qt/objectid.h>

namespace GIT {

class FetchHead
{
public:
    FetchHead() {}
    FetchHead(const QString& remoteCanonicalName, const QString& remoteUrl, const ObjectId& objectId, bool isMerge) :
        _remoteCanonicalName(remoteCanonicalName), _remoteUrl(remoteUrl), _objectId(objectId), _isMerge(isMerge) {}

    QString remoteCanonicalName() const { return _remoteCanonicalName; }
    QString remoteUrl() const { return _remoteUrl; }
    ObjectId objectId() const { return _objectId; }
    bool isMerge() const { return _isMerge; }

    class List : public QList<FetchHead>
    {
    public:
        List findForMerge() const
        {
            List result;
            for(const FetchHead& fetchHead : *this) {
                if(fetchHead.isMerge()) {
                    result.append(fetchHead);
                }
            }
            return result;
        }
    };

private:
    QString _remoteCanonicalName;
    QString _remoteUrl;
    ObjectId _objectId;
    bool _isMerge = false;
};

} // namespace GIT

#endif // FETCHHEAD_H
