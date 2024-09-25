/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This is a signature for a commit, containing name, email and timestamp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef SIGNATURE_H
#define SIGNATURE_H
#include <git2qt/gitentity.h>

#include <QDateTime>

namespace GIT {

class Repository;
class GIT2QT_EXPORT Signature : public GitEntity
{
public:
    Signature() :
        GitEntity(SignatureEntity) {}
    Signature(const QString& name, const QString& email, const QDateTime& timestamp = QDateTime::currentDateTimeUtc()) :
        GitEntity(SignatureEntity),
        _name(name), _email(email), _timestamp(timestamp) {}
    Signature(const git_signature* nativeSignature);
    virtual ~Signature();

    QString name() const { return _name; }
    void setName(const QString& value) { _name = value; }

    QString email() const { return _email; }
    void setEmail(const QString& value) { _email = value; }

    QDateTime timestamp() const { return _timestamp; }
    void setTimestamp(const QDateTime& value) { _timestamp = value; }

    virtual bool isNull() const { return _native == nullptr; }

    const git_signature* toNative();

private:
    QString _name;
    QString _email;
    QDateTime _timestamp;

    git_signature* _native = nullptr;
};

} // namespace GIT

#endif // SIGNATURE_H
