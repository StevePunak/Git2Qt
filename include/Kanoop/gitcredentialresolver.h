#ifndef GITCREDENTIALRESOLVER_H
#define GITCREDENTIALRESOLVER_H
#include <Kanoop/kanoopgit.h>
#include <QObject>

class KANOOPGIT_EXPORT GitCredentialResolver
{
public:
    GitCredentialResolver() {}
    virtual ~GitCredentialResolver() {}

    /**
     * Override these functions to provide interface functionality
     */
    virtual bool getUsernameAndPassword() { return false; }

    /**
     * Public getters
     */
    QString username() const { return _username; }
    QString password() const { return _password; }

protected:
    /**
     * Protected setters
     */
    void setUsername(const QString& value) { _username = value; }
    void setPassword(const QString& value) { _password = value; }

private:
    QString _username;
    QString _password;
};

#endif // GITCREDENTIALRESOLVER_H
