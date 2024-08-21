#include "configuration.h"

#include "log.h"
#include <utility.h>

#include <configurationentry.h>
#include <gitexception.h>
#include <repository.h>
#include <repositoryinformation.h>

#include <git2/repository.h>

using namespace GIT;

Configuration::Configuration(Repository* repo) :
    GitEntity(ConfigurationEntity, repo)
{
    reload();
}

Configuration::~Configuration()
{
    _handle.dispose();
}

Signature Configuration::buildSignature(const QDateTime& timestamp)
{
    Signature result;
    QVariant name = get("user.name").value();
    QVariant email = get("user.email").value();
    if(name.toString().isEmpty() == false && email.toString().isEmpty() == false) {
        result = Signature(name.toString(), email.toString(), timestamp);
    }
    return result;
}

void Configuration::reload()
{
    try
    {
        _handle.dispose();

        git_config* config = nullptr;
        if(git_config_new(&config) == 0) {
            _handle = ConfigurationHandle(config);
        }

        if(_handle.isNull()) {
            throw GitException("Failed to create handle for config");
        }

        RepositoryHandle repoHandle = repository()->handle();

        git_buf buf = GIT_BUF_INIT;
        if(git_config_find_global(&buf) == 0) {
            _globalConfigPath = buf.ptr;
        }

        if(git_config_find_xdg(&buf) == 0) {
            _xdgConfigPath = buf.ptr;
        }

        if(git_config_find_system(&buf) == 0) {
            _systemConfigPath = buf.ptr;
        }

        if(git_config_find_programdata(&buf) == 0) {
            _programDataConfigPath = buf.ptr;
        }

        QString path = Utility::combine(repository()->info()->path(), "config");
        git_config_add_file_ondisk(_handle.value(), path.toUtf8().constData(), (git_config_level_t)Local, repoHandle.value(), true);
        _repoConfigPath = path;


        if (_globalConfigPath.isEmpty() == false) {
            git_config_add_file_ondisk(_handle.value(), _globalConfigPath.toUtf8().constData(), (git_config_level_t)Global, repository()->handle().value(), true);
        }

        if (_xdgConfigPath.isEmpty() == false) {
            git_config_add_file_ondisk(_handle.value(), _xdgConfigPath.toUtf8().constData(), (git_config_level_t)Xdg, repository()->handle().value(), true);
        }

        if (_systemConfigPath.isEmpty() == false) {
            git_config_add_file_ondisk(_handle.value(), _systemConfigPath.toUtf8().constData(), (git_config_level_t)System, repository()->handle().value(), true);
        }

        if (_programDataConfigPath.isEmpty() == false) {
            git_config_add_file_ondisk(_handle.value(), _programDataConfigPath.toUtf8().constData(), (git_config_level_t)ProgramData, repository()->handle().value(), true);
        }
    }
    catch(const GitException&)
    {
    }
}

ConfigurationHandle Configuration::createHandle(ConfigurationLevel level) const
{
    ConfigurationHandle newHandle;
    if(_handle.isNull() == false) {
        git_config* config = nullptr;
        if(git_config_open_level(&config, _handle.value(), (git_config_level_t)level) == 0) {
            newHandle = ConfigurationHandle(config);
        }
    }
    return newHandle;
}

ConfigurationEntry Configuration::get(const QString& keyP1, const QString& keyP2, const QString& keyP3)
{
    ConfigurationEntry result;

    git_config* snapshot = nullptr;
    git_config_entry* entry = nullptr;
    QString key = keyP1;
    if(keyP2.isEmpty() == false) {
        key += QString(".%1").arg(keyP2);
    }
    if(keyP3.isEmpty() == false) {
        key += QString(".%1").arg(keyP3);
    }

    try
    {
        throwIfTrue(_handle.isNull());
        throwOnError(git_config_snapshot(&snapshot, _handle.value()));
        throwOnError(git_config_get_entry(&entry, snapshot, key.toUtf8().constData()));
        result = ConfigurationEntry(entry->name, entry->value, (ConfigurationLevel)entry->level);
    }
    catch(const GitException&)
    {

    }

    if(snapshot != nullptr) {
        git_config_free(snapshot);
    }
    if(entry != nullptr) {
        git_config_entry_free(entry);
    }
    return result;
}

ConfigurationEntry::List Configuration::getAll()
{
    ConfigurationEntry::List result;
    git_config_iterator* it;
    git_config_entry* entry;
    if(_handle.isNull() == false) {
        int error = git_config_iterator_new(&it, _handle.value());
        while(error == false && git_config_next(&entry, it) == 0) {
            ConfigurationEntry configEntry(entry->name, entry->value, (ConfigurationLevel)entry->level);
            result.append(configEntry);
        }
        git_config_iterator_free(it);
    }
    return result;
}

bool Configuration::set(const QString& key, const QString& value, ConfigurationLevel level)
{
    bool result = false;
    try
    {
        ConfigurationHandle levelHandle = createHandle(level);
        throwIfTrue(levelHandle.isNull());
        throwOnError(git_config_set_string(levelHandle.value(), key.toUtf8().constData(), value.toUtf8().constData()));
        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }
    return result;
}

void Configuration::dumpToLog(git_config* config)
{
    git_config_iterator* it;
    git_config_entry* entry;
    int error = git_config_iterator_new(&it, config);
    while(error == false && git_config_next(&entry, it) == 0) {
        Log::logText(LVL_DEBUG, QString("%1 = %2").arg(entry->name).arg(entry->value));
    }
    git_config_iterator_free(it);
}
