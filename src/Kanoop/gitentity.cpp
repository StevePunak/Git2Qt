#include "gitentity.h"

#include <Kanoop/commonexception.h>

#include <repository.h>

using namespace GIT;

bool GitEntity::handleError(int value)
{
    if(value) {
        setErrorText(git_error_last()->message);
        return true;
    }
    return false;
}

void GitEntity::throwOnError(int result)
{
    if(result != 0) {
        setErrorText(git_error_last()->message);
        throw CommonException(_errorText);
    }
}

void GitEntity::throwIfNull(const void* ptr, const QString& message)
{
    if(ptr == nullptr) {
        setErrorText(message.isEmpty() ? git_error_last()->message : message);
        throw CommonException(message);
    }
}

void GitEntity::throwIfFalse(bool result, const QString& message)
{
    if(result == false) {
        setErrorText(message.isEmpty() ? git_error_last()->message : message);
        throw CommonException(message);
    }
}

void GitEntity::throwIfEmpty(const QString& value, const QString& message)
{
    if(value.isEmpty()) {
        setErrorText(message.isEmpty() ? git_error_last()->message : message);
        throw CommonException(message);
    }
}

void GitEntity::throwOnError(Repository* repo, int result)
{
    repo->throwOnError(result);
}

void GitEntity::setErrorText(const QString& errorText)
{
    _errorText = errorText;
    if(_type != RepositoryEntity && _repository != nullptr) {
        _repository->setErrorText(errorText);
    }
}

GitEntity& GitEntity::operator=(const GitEntity& other)
{
    _type = other._type;
    _repository = other._repository;
    _errorText = other._errorText;
    return *this;
}

