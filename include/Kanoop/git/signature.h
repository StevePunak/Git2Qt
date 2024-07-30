#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <Kanoop/git/gitentity.h>

#include <QDateTime>

namespace GIT {

class Repository;
class Signature
{
public:
    Signature() {}
    Signature(const QString& name, const QString& email, const QDateTime& timestamp = QDateTime::currentDateTimeUtc()) :
        _name(name), _email(email), _timestamp(timestamp) {}
    Signature(const git_signature* nativeSignature);
    virtual ~Signature();

    QString name() const { return _name; }
    QString email() const { return _email; }
    QDateTime timestamp() const { return _timestamp; }

    const git_signature* toNative();

private:
    QString _name;
    QString _email;
    QDateTime _timestamp;

    git_signature* _native = nullptr;
};

} // namespace GIT

#endif // SIGNATURE_H
