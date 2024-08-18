/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is intended to be subclassed to provide callbacks for credential
 * resolution during authentication to a remote repository.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef CREDENTIALRESOLVER_H
#define CREDENTIALRESOLVER_H
#include <Kanoop/git/kanoopgit.h>
#include <QObject>

namespace GIT {

class KANOOPGIT_EXPORT CredentialResolver
{
public:
    CredentialResolver() {}
    virtual ~CredentialResolver() {}

    /**
     * Public getters
     *
     * Override the virtual 'get' methods to provide interactive behavior
     */
    virtual QString getUsername() const { return _username; }
    void setUsername(const QString& value) { _username = value; }

    virtual QString getPassword() const { return _password; }
    void setPassword(const QString& value) { _password = value; }

    virtual QString getPublicKeyFilename() const { return _publicKeyFilename; }
    void setPublicKeyFilename(const QString& value) { _publicKeyFilename = value; }

    virtual QString getPrivateKeyFilename() const { return _privateKeyFilename; }
    void setPrivateKeyFilename(const QString& value) { _privateKeyFilename = value; }

private:
    QString _username;
    QString _password;
    QString _publicKeyFilename;
    QString _privateKeyFilename;
};

} // namespace GIT

#endif // CREDENTIALRESOLVER_H
