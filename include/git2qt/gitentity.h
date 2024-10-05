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
#include <git2qt/log.h>
#include <git2qt/gittypes.h>
#include <git2qt/declspec.h>

namespace GIT {

class Repository;
class GIT2QT_EXPORT GitEntity
{
public:
    virtual ~GitEntity() {}

    GitEntityType entityType() const { return _objectType; }

    Repository* repository() const { return _repository; }

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

    void throwOnError(int result, const QString& message = QString()) const;
    void throwIfNull(const void* ptr, const QString& message = QString()) const;
    void throwIfFalse(bool result, const QString& message = QString()) const;
    void throwIfTrue(bool result, const QString& message = QString()) const { return throwIfFalse(!result, message); }
    void throwIfEmpty(const QString& value, const QString& message = QString()) const;

    void logText(const char* file, int line, Log::LogLevel level, const QString& text) const;

    static void throwOnError(Repository* repo, int result);
    static void throwIfTrue(Repository* repo, int result);
    static void throwIfNull(Repository* repo, const void* ptr);

    void setRepository(Repository* value) { _repository = value; }

private:
    void throwException(const QString& message, int errorCode = 0) const;

    GitEntityType _objectType = UnknownGitEntityType;
    Repository* _repository = nullptr;
};

} // namespace GIT

#endif // GITENTITY_H
