#ifndef CONFIGURATIONENTRY_H
#define CONFIGURATIONENTRY_H
#include <Kanoop/git/gittypes.h>

#include <QString>
#include <QVariant>

namespace GIT {

class ConfigurationEntry
{
public:
    ConfigurationEntry() {}
    ConfigurationEntry(const QString& key, const QVariant& value, ConfigurationLevel level) :
        _key(key), _value(value), _level(level) {}

    QString key() const { return _key; }
    QVariant value() const { return _value; }
    ConfigurationLevel level() const { return _level; }

    bool isValid() const { return _value.isValid(); }

    class List : public QList<ConfigurationEntry> {};

private:
    QString _key;
    QVariant _value;
    ConfigurationLevel _level;
};

} // namespace GIT

#endif // CONFIGURATIONENTRY_H
