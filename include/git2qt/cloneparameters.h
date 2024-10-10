#ifndef CLONEPARAMETERS_H
#define CLONEPARAMETERS_H

#include <QString>

namespace GIT {

class ProgressCallback;

class AbstractCredentialResolver;

class CloneParameters
{
public:
    CloneParameters(AbstractCredentialResolver* credentialResolver, ProgressCallback* progressCallback) :
        _credentialResolver(credentialResolver), _progressCallback(progressCallback) {}

    AbstractCredentialResolver* credentialResolver() const { return _credentialResolver; }
    ProgressCallback* progressCallback() const { return _progressCallback; }

    QString errorText() const { return _errorText; }
    void setErrorText(const QString& value) { _errorText = value; }

private:
    AbstractCredentialResolver* _credentialResolver;
    ProgressCallback* _progressCallback;
    QString _errorText;
};

} // namespace GIT

#endif // CLONEPARAMETERS_H
