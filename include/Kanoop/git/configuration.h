#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/gitentity.h>
#include <QDateTime>

namespace GIT {

class ConfigurationEntry;
class Signature;
class Configuration : public GitEntity
{
public:
    Configuration(Repository* repo);

    QString repoConfigPath() const { return _repoConfigPath; }
    QString globalConfigPath() const { return _globalConfigPath; }
    QString xdgConfigPath() const { return _xdgConfigPath; }
    QString systemConfigPath() const { return _systemConfigPath; }
    QString programDataConfigPath() const { return _programDataConfigPath; }

    Signature buildSignature(const QDateTime& timestamp = QDateTime::currentDateTimeUtc());
    ConfigurationEntry get(const QString& key);

    virtual bool isNull() const { return _handle == nullptr; }
private:
    QString _repoConfigPath;
    QString _globalConfigPath;
    QString _xdgConfigPath;
    QString _systemConfigPath;
    QString _programDataConfigPath;

    git_config* _handle;
};

} // namespace GIT

#endif // CONFIGURATION_H
