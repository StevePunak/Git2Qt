/**
 * Copyright (c) 2024 Stephen Punak
 *
 * QException subclass for libgit2qt
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GITEXCEPTION_H
#define GITEXCEPTION_H

#include <QException>
#include <QString>

namespace GIT {

class GitException : public QException
{
public:
    GitException(const QString& message, qint32 code = 0) :
        _message(message),
        _code(code) {}

    void raise() const override { throw *this; }
    GitException *clone() const override { return new GitException(*this); }

    QString message() const { return _message; }
    qint32 code() const { return _code; }

private:
    QString _message;
    qint32 _code;
};

} // namespace

#endif // GITEXCEPTION_H
