#ifndef REFERENCE_H
#define REFERENCE_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/objectid.h>

#include <QMap>

namespace GIT {

class Repository;
class DirectReference;
class SymbolicReference;
class Reference : public GitEntity
{
public:
    enum ReferenceType {
        UnknownReferenceType,
        SymbolicReferenceType,
        DirectReferenceType,
    };

    static Reference* create(Repository* repo, git_reference* handle);
    static Reference* create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite);
    static Reference* lookup(Repository* repo, const QString& name);

    QString name() const;
    ReferenceType type() const;

    QString canonicalName() const { return _canonicalName; }
    QString targetIdentifier() const { return _targetIdentifier; }
    ObjectId objectId() const;

    git_reference* handle() const { return _reference; }

    virtual bool isDirect() const { return type() == DirectReferenceType; }
    virtual bool isSymbolic() const { return type() == SymbolicReferenceType; }
    virtual bool isNull() const override { return _reference == nullptr; }

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

    class Map : public QMap<QString, Reference*>
    {
    public:
        Reference* findByObjectId(const ObjectId& objectId) const
        {
            Reference* result = nullptr;
            auto it = std::find_if(constBegin(), constEnd(), [objectId](Reference* r) { return r->objectId() == objectId; });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }
    };

protected:
    explicit Reference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier);

public:
    virtual ~Reference();

private:
    static ReferenceType typeFromHandle(git_reference* handle);
    static QString nameFromHandle(git_reference* handle);
    static QString symbolicTargetNameFromHandle(git_reference* handle);
    static ObjectId objectIdFromHandle(git_reference* handle);

    git_reference* _reference = nullptr;
    QString _canonicalName;
    QString _targetIdentifier;

public:
    static const QString LocalBranchPrefix;
    static const QString RemoteTrackingBranchPrefix;
    static const QString TagPrefix;
    static const QString NotePrefix;
};

class SymbolicReference : public Reference
{
public:
    explicit SymbolicReference(Repository* repo, const QString& canonicalName, const QString& targetIdentifier, Reference* target);

    Reference* target() const { return _target; }
    void setTarget(Reference* value) { _target = value; }

private:
    Reference* _target = nullptr;
};

class DirectReference : public Reference
{
public:
    explicit DirectReference(Repository* repo, const QString& canonicalName, const ObjectId& targetoid);

private:
    ObjectId _targetOid;
};

} // namespace GIT

#endif // REFERENCE_H
