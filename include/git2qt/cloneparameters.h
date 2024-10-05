#ifndef CLONEPARAMETERS_H
#define CLONEPARAMETERS_H

#include <QString>

namespace GIT {

class ProgressCallback;

class CredentialResolver;

class CloneParameters
{
public:
    CloneParameters(CredentialResolver* credentialResolver, ProgressCallback* progressCallback) :
        _credentialResolver(credentialResolver), _progressCallback(progressCallback) {}

    CredentialResolver* credentialResolver() const { return _credentialResolver; }
    ProgressCallback* progressCallback() const { return _progressCallback; }

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& value) { _errorText = value; }

private:
    CredentialResolver* _credentialResolver;
    ProgressCallback* _progressCallback;
    QString _errorText;
};

} // namespace GIT

#endif // CLONEPARAMETERS_H
