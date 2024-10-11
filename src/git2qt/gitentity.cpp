#include "gitentity.h"

#include <gitexception.h>
#include <log.h>
#include <repository.h>

using namespace GIT;

void GitEntity::throwOnError(int result, const QString& message) const
{
    if(result != 0) {
        throwException(message, result);
    }
}

void GitEntity::throwIfNull(const void* ptr, const QString& message) const
{
    if(ptr == nullptr) {
        throwException(message);
    }
}

void GitEntity::throwIfFalse(bool result, const QString& message) const
{
    if(result == false) {
        throwException(message);
    }
}

void GitEntity::throwIfEmpty(const QString& value, const QString& message) const
{
    if(value.isEmpty()) {
        throwException(message);
    }
}

void GitEntity::logText(const char* file, int line, Log::LogLevel level, const QString& text) const
{
    Log::logText(file, line, level, text);
}

void GitEntity::throwOnError(Repository* repo, int result)
{
    repo->throwOnError(result);
}

void GitEntity::throwIfTrue(Repository* repo, int result)
{
    repo->throwIfTrue(result);
}

void GitEntity::throwIfFalse(Repository* repo, int result, const QString& message)
{
    repo->throwIfFalse(result, message);
}

void GitEntity::throwIfNull(Repository* repo, const void* ptr, const QString& message)
{
    repo->throwIfNull(ptr, message);
}

void GitEntity::throwException(const QString& message, int errorCode) const
{
    QString errorText = message;
    if(errorText.isEmpty()) {
        const git_error* err = git_error_last();
        if(err != nullptr) {
            errorText = err->message;
        }
    }
    repository()->setErrorText(errorText);
    repository()->setErrorCode(errorCode);
    throw GitException(errorText);
}

bool GitEntity::operator ==(const GitEntity& other) const
{
    return _objectType == other._objectType;
}

void GitEntity::setErrorText(const QString& errorText)
{
    if(_objectType != RepositoryEntity && _repository != nullptr) {
        _repository->setErrorText(errorText);
    }
}

GitEntity& GitEntity::operator=(const GitEntity& other)
{
    _objectType = other._objectType;
    _repository = other._repository;
    return *this;
}

