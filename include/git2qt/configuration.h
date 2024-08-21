/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class provides access to the various levels of git configuration.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <git2qt/gittypes.h>
#include <git2qt/gitentity.h>
#include <git2qt/handle.h>
#include <git2qt/configurationentry.h>
#include <QDateTime>

namespace GIT {

class Signature;
class Configuration : public QObject,
                      public GitEntity
{
    Q_OBJECT
public:
    Configuration(Repository* repo);
    virtual ~Configuration();

    // Getters
    ConfigurationEntry get(const QString& keyP1, const QString& keyP2 = QString(), const QString& keyP3 = QString());
    ConfigurationEntry::List getAll();

    // Setters
    bool set(const QString& key, const QString& value, ConfigurationLevel level);

    Signature buildSignature(const QDateTime& timestamp = QDateTime::currentDateTimeUtc());

    QString repoConfigPath() const { return _repoConfigPath; }
    QString globalConfigPath() const { return _globalConfigPath; }
    QString xdgConfigPath() const { return _xdgConfigPath; }
    QString systemConfigPath() const { return _systemConfigPath; }
    QString programDataConfigPath() const { return _programDataConfigPath; }

    virtual bool isNull() const { return _handle.isNull(); }

public slots:
    void reload();

private:
    ConfigurationHandle createHandle(ConfigurationLevel level) const;
    void dumpToLog(git_config* config);

    QString _repoConfigPath;
    QString _globalConfigPath;
    QString _xdgConfigPath;
    QString _systemConfigPath;
    QString _programDataConfigPath;

    ConfigurationHandle _handle;
};

} // namespace GIT

#endif // CONFIGURATION_H
