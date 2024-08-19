/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Base class for all git entities in this library.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GITENTITY_H
#define GITENTITY_H

#include <QString>
#include <git2.h>

namespace GIT {

class Repository;
class GitEntity
{
public:
    enum GitEntityType {
        UnknownGitEntityType = 0,

        BlobEntity,
        BranchEntity,
        BranchCollectionEntity,
        CommitEntity,
        RepositoryEntity,
        ReferenceEntity,
        ReferenceCollectionEntity,
        DiffEntity,
        TreeEntity,
        IndexEntity,
        ConfigurationEntity,
        RepositoryInfoEntity,
        ObjectDatabaseEntity,
        NetworkEntity,
        RemoteEntity,
        RemoteCollectionEntity,
        SignatureEntity,
        SubmoduleEntity,
        SubmoduleCollectionEntity,
        TagLightweightEntity,
        TagAnnotatedEntity,
        TagCollectionEntity,
        TreeEntryEntity,
    };

    virtual ~GitEntity() {}

    GitEntityType entityType() const { return _objectType; }

    Repository* repository() const { return _repository; }

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& errorText);

    bool isBlob() const { return entityType() == BlobEntity; }
    bool isCommit() const { return entityType() == CommitEntity; }
    bool isTag() const { return entityType() == TagLightweightEntity; }
    bool isTree() const { return entityType() == TreeEntity; }

    virtual bool isNull() const = 0;

protected:
    GitEntity(GitEntityType type, Repository* repo = nullptr) :
        _objectType(type), _repository(repo) {}
    GitEntity(const GitEntity& other) { *this = other; }

    GitEntity& operator=(const GitEntity& other);

    bool handleError(int value);
    void throwOnError(int result, const QString& message = QString());
    void throwIfNull(const void* ptr, const QString& message = QString());
    void throwIfFalse(bool result, const QString& message = QString());
    void throwIfTrue(bool result, const QString& message = QString()) { return throwIfFalse(!result, message); }
    void throwIfEmpty(const QString& value, const QString& message = QString());

    static void throwOnError(Repository* repo, int result);

    void setRepository(Repository* value) { _repository = value; }

private:
    void throwException(const QString& message);

    GitEntityType _objectType = UnknownGitEntityType;
    Repository* _repository = nullptr;

    QString _errorText;
};

} // namespace GIT

#endif // GITENTITY_H
