#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/handle.h>
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
    ConfigurationEntry get(const QString& keyP1, const QString& keyP2 = QString(), const QString& keyP3 = QString());

    virtual bool isNull() const { return createHandle().isNull(); }

private:
    ConfigurationHandle createHandle() const;

    QString _repoConfigPath;
    QString _globalConfigPath;
    QString _xdgConfigPath;
    QString _systemConfigPath;
    QString _programDataConfigPath;
};

} // namespace GIT

#endif // CONFIGURATION_H
