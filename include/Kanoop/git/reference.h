#ifndef REFERENCE_H
#define REFERENCE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/objectid.h>
#include <Kanoop/git/handle.h>

#include <QMap>

namespace GIT {

class Repository;
class Reference : public GitEntity
{
public:
    Reference() :
        GitEntity(ReferenceEntity) {}

    Reference(const Reference& other);
    Reference& operator=(const Reference& other);

    ReferenceHandle createHandle() const;

    static Reference create(Repository* repo, git_reference* handle);
    static Reference create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite);
    static Reference createDirectReferenceObject(Repository* repo, const QString& canonicalName, const ObjectId& targetoid);
    static Reference createSymbolicReferenceObject(Repository* repo, const QString& canonicalName, const QString& targetIdentifier);
    static Reference lookup(Repository* repo, const QString& name);

    QString name() const;
    ReferenceType type() const;

    QString canonicalName() const { return _canonicalName; }
    QString targetIdentifier() const { return _targetIdentifier; }
    ObjectId objectId() const;

    Reference* target() const { return _target; }
    ObjectId targetOid() const { return _targetOid; }

    void resolveTarget();

    virtual bool isDirect() const { return type() == DirectReferenceType; }
    virtual bool isSymbolic() const { return type() == SymbolicReferenceType; }
    virtual bool isNull() const override;

    bool looksLikeLocalBranch() const { return looksLikeLocalBranch(_canonicalName); }
    bool looksLikeRemoteTrackingBranch() const { return looksLikeRemoteTrackingBranch(_canonicalName); }
    bool looksLikeTag() const { return looksLikeTag(_canonicalName); }
    bool looksLikeNote() const { return looksLikeNote(_canonicalName); }
    bool isPrefixedBy(const QString& prefix) const { return isPrefixedBy(_canonicalName, prefix); }

    static bool looksLikeLocalBranch(const QString& canonicalName) { return isPrefixedBy(canonicalName, LocalBranchPrefix); }
    static bool looksLikeRemoteTrackingBranch(const QString& canonicalName) { return isPrefixedBy(canonicalName, RemoteTrackingBranchPrefix); }
    static bool looksLikeTag(const QString& canonicalName) { return isPrefixedBy(canonicalName, TagPrefix); }
    static bool looksLikeNote(const QString& canonicalName) { return isPrefixedBy(canonicalName, NotePrefix); }
    static bool isPrefixedBy(const QString& value, const QString& prefix) { return value.startsWith(prefix); }

    class Map : public QMap<QString, Reference>
    {
    public:
        Reference findByObjectId(const ObjectId& objectId) const
        {
            Reference result;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](const Reference& r) { return r.objectId() == objectId; });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

protected:
    explicit Reference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier, ReferenceType referenceType);

public:
    virtual ~Reference();

private:
    static ReferenceType typeFromHandle(const ReferenceHandle& handle);
    static QString nameFromHandle(const ReferenceHandle& handle);
    static QString symbolicTargetNameFromHandle(const ReferenceHandle& handle);
    static ObjectId objectIdFromHandle(const ReferenceHandle& handle);


    QString _canonicalName;
    QString _targetIdentifier;
    ReferenceType _type;

    Reference* _target = nullptr;
    ObjectId _targetOid;

public:
    static const QString LocalBranchPrefix;
    static const QString RemoteTrackingBranchPrefix;
    static const QString TagPrefix;
    static const QString NotePrefix;
};

} // namespace GIT

#endif // REFERENCE_H
