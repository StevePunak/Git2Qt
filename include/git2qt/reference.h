/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git-reference.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REFERENCE_H
#define REFERENCE_H
#include <git2qt/gitentity.h>
#include <git2qt/gittypes.h>
#include <git2qt/objectid.h>
#include <git2qt/handle.h>

#include <QMap>
#include <QRegularExpressionMatch>

namespace GIT {

class GitObject;

class Commit;

class Repository;
class GIT2QT_EXPORT Reference : public GitEntity
{
public:
    Reference() :
        GitEntity(ReferenceEntity) {}

    Reference(const Reference& other);
    Reference& operator=(const Reference& other);
    bool operator ==(const Reference& other) const;

    ReferenceHandle createHandle() const;

    static Reference create(Repository* repo, git_reference* handle);
    static Reference create(Repository* repo, const QString& name, const ObjectId& targetId, const QString& logMessage, bool allowOverwrite);
    static Reference createDirectReferenceObject(Repository* repo, const QString& canonicalName, const ObjectId& targetoid);
    static Reference createSymbolicReferenceObject(Repository* repo, const QString& canonicalName, const QString& targetIdentifier);
    static Reference lookup(Repository* repo, const QString& name);

    QString name() const;
    ReferenceType type() const { return _type; }

    QString canonicalName() const { return _canonicalName; }
    QString friendlyName() const;
    QString targetIdentifier() const { return _targetIdentifier; }
    ObjectId objectId() const;

    Reference target() const { return _target != nullptr ? *_target : Reference(); }
    ObjectId targetObjectId() const { return _targetOid; }

    Reference resolveToDirectReference() const;
    Commit peelToCommit() const;

    void resolveTarget();

    int depth() const;

    bool isBranch() const { return _isBranch; }
    bool isNote() const { return _isNote; }
    bool isRemote() const { return _isRemote; }
    bool isLocal() const { return !_isRemote; }
    bool isTag() const { return _isTag; }

    virtual bool isDirect() const { return type() == DirectReferenceType; }
    virtual bool isSymbolic() const { return type() == SymbolicReferenceType; }
    virtual bool isNull() const override;

    QVariant toVariant() const { return QVariant::fromValue<Reference>(*this); }
    static Reference fromVariant(const QVariant& value) { return value.value<Reference>(); }

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
    void resolveProperties();

    static ReferenceType typeFromHandle(const ReferenceHandle& handle);
    static QString nameFromHandle(const ReferenceHandle& handle);
    static QString symbolicTargetNameFromHandle(const ReferenceHandle& handle);
    static ObjectId objectIdFromHandle(const ReferenceHandle& handle);


    QString _canonicalName;
    QString _targetIdentifier;
    ReferenceType _type;

    bool _isBranch = false;
    bool _isNote = false;
    bool _isRemote = false;
    bool _isTag = false;

    Reference* _target = nullptr;
    ObjectId _targetOid;

public:
    static const QString LocalBranchPrefix;
    static const QString RemoteTrackingBranchPrefix;
    static const QString TagPrefix;
    static const QString NotePrefix;
};

class ReferenceList : public QList<Reference>
{
public:
    ReferenceList() {}
    ReferenceList(const QList<Reference>& other)
    {
        for(const Reference& r : other) {
            append(r);
        }
    }

    QVariant toVariant() const { return QVariant::fromValue<ReferenceList>(*this); }
    static ReferenceList fromVariant(const QVariant& value) { return value.value<ReferenceList>(); }

    Reference findByObjectId(const ObjectId& objectId) const
    {
        Reference result;
        for(const Reference& reference : *this) {
            if(reference.isDirect() && reference.objectId() == objectId) {
                result = reference;
                break;
            }
        }
        return result;
    }

    Reference findByCanonicalName(const QString& canonicalName) const
    {
        Reference result;
        auto it = std::find_if(constBegin(), constEnd(), [canonicalName](const Reference& r) { return r.canonicalName() == canonicalName; });
        if(it != constEnd()) {
            result = *it;
        }
        return result;
    }

    ReferenceList findReferences(const QRegularExpression& regex) const
    {
        ReferenceList result;

        for(const Reference& reference : *this) {
            QRegularExpressionMatch match = regex.match(reference.canonicalName());
            if(match.hasMatch()) {
                result.append(reference);
            }
        }

        return result;
    }

    ReferenceList findByTargetObjectId(const ObjectId& objectId) const
    {
        ReferenceList result;
        for(const Reference& reference : *this) {
            if(reference.targetObjectId() == objectId) {
                result.append(reference);
            }
        }
        return result;
    }

    ReferenceList localBranchReferences() const
    {
        ReferenceList result;
        for(const Reference& reference : *this) {
            if(reference.isBranch() == true && reference.isRemote() == false) {
                result.append(reference);
            }
        }
        return result;
    }

    ObjectId::List objectIds() const
    {
        ObjectId::List result;
        for(const Reference& reference : *this) {
            if(reference.isDirect()) {
                result.append(reference.objectId());
            }
            else {
                result.append(reference.targetObjectId());
            }
        }
        return result;
    }

};

} // namespace GIT

Q_DECLARE_METATYPE(GIT::Reference)
Q_DECLARE_METATYPE(GIT::ReferenceList)

#endif // REFERENCE_H
