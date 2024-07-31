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
        UnknownGitObjectType = 0,

        BranchEntity,
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
    };

    virtual ~GitEntity() {}

    GitEntityType type() const { return _type; }

    Repository* repository() const { return _repository; }

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& errorText);

    virtual bool isNull() const = 0;

protected:
    GitEntity(GitEntityType type, Repository* repo = nullptr) :
        _type(type), _repository(repo) {}

    bool handleError(int value);
    void throwOnError(int result);
    void throwIfNull(const void* ptr, const QString& message = QString());
    void throwIfFalse(bool result, const QString& message = QString());
    void throwIfTrue(bool result, const QString& message = QString()) { return throwIfFalse(!result, message); }
    void throwIfEmpty(const QString& value, const QString& message = QString());
    void setRepository(Repository* value) { _repository = value; }

private:
    GitEntityType _type = UnknownGitObjectType;
    Repository* _repository = nullptr;

    QString _errorText;
};

} // namespace GIT

#endif // GITENTITY_H
