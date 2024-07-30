#ifndef GITENTITY_H
#define GITENTITY_H

#include <QObject>
#include <QString>
#include <git2.h>

namespace GIT {

class Repository;
class GitEntity : public QObject
{
    Q_OBJECT
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
    };

    virtual ~GitEntity() {}

    GitEntityType type() const { return _type; }

    Repository* repository() const { return _repository; }

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& errorText);

    virtual bool isNull() const = 0;

protected:
    GitEntity(GitEntityType type, Repository* repo = nullptr) :
        QObject(),
        _type(type), _repository(repo) {}

    bool handleError(int value);
    void throwOnError(int result);
    void throwIfNull(void* ptr, const QString& message = QString());
    void throwIfFalse(bool result, const QString& message = QString());
    void setRepository(Repository* value) { _repository = value; }

private:
    GitEntityType _type = UnknownGitObjectType;
    Repository* _repository = nullptr;

    QString _errorText;
};

} // namespace GIT

#endif // GITENTITY_H
