#include "configuration.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/klog.h>
#include <Kanoop/pathutil.h>

#include <configurationentry.h>
#include <repository.h>
#include <repositoryinformation.h>

#include <git2/repository.h>

using namespace GIT;

Configuration::Configuration(Repository* repo) :
    GitEntity(ConfigurationEntity, repo)
{
    git_config* config = nullptr;
    if(git_config_new(&config) == 0) {
        _handle = ConfigurationHandle(config);
        try
        {
            if(_handle.isNull()) {
                throw CommonException("Failed to create handle for config");
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

            QString path = PathUtil::combine(repo->info()->path(), "config");
            git_config_add_file_ondisk(_handle.value(), path.toUtf8().constData(), (git_config_level_t)Local, repoHandle.value(), true);
            _repoConfigPath = path;

            // deprecated?
            // git_repository_set_config(repository()->handle(), _handle);

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
        catch(CommonException&)
        {
        }

    }
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
    catch(const CommonException&)
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

void Configuration::dumpToLog(git_config* config)
{
    git_config_iterator* it;
    git_config_entry* entry;
    int error = git_config_iterator_new(&it, config);
    while(error == false && git_config_next(&entry, it) == 0) {
        KLog::sysLogText(KLOG_DEBUG, QString("%1 = %2").arg(entry->name).arg(entry->value));
    }
}
