#include "gitentity.h"

#include <gitexception.h>
#include <log.h>
#include <repository.h>

using namespace GIT;

bool GitEntity::handleError(int value)
{
    if(value) {
        repository()->setErrorText(git_error_last()->message);
        return true;
    }
    return false;
}

void GitEntity::throwOnError(int result, const QString& message) const
{
    if(result != 0) {
        throwException(message);
    }
}

void GitEntity::throwIfNull(const void* ptr, const QString& message)
{
    if(ptr == nullptr) {
        throwException(message);
    }
}

void GitEntity::throwIfFalse(bool result, const QString& message)
{
    if(result == false) {
        throwException(message);
    }
}

void GitEntity::throwIfEmpty(const QString& value, const QString& message)
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

void GitEntity::throwException(const QString& message) const
{
    QString errorText = message;
    if(errorText.isEmpty()) {
        const git_error* err = git_error_last();
        if(err != nullptr) {
            errorText = err->message;
        }
    }
    repository()->setErrorText(errorText);
    throw GitException(errorText);
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

