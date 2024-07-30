#include "configuration.h"

#include <Kanoop/commonexception.h>
#include <Kanoop/pathutil.h>

#include <configurationentry.h>
#include <repository.h>
#include <repositoryinformation.h>

#include <git2/repository.h>

using namespace GIT;

Configuration::Configuration(Repository* repo) :
    GitEntity(ConfigurationEntity, repo)
{
    if(git_config_new(&_handle) != 0) {
        _handle = nullptr;
    }
    else {
        try
        {
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
            git_config_add_file_ondisk(_handle, path.toUtf8().constData(), (git_config_level_t)Local, repository()->handle(), true);
            _repoConfigPath = path;

            // deprecated?
            // git_repository_set_config(repository()->handle(), _handle);

            if (_globalConfigPath.isEmpty() == false) {
                git_config_add_file_ondisk(_handle, _globalConfigPath.toUtf8().constData(), (git_config_level_t)Global, repository()->handle(), true);
            }

            if (_xdgConfigPath.isEmpty() == false) {
                git_config_add_file_ondisk(_handle, _xdgConfigPath.toUtf8().constData(), (git_config_level_t)Xdg, repository()->handle(), true);
            }

            if (_systemConfigPath.isEmpty() == false) {
                git_config_add_file_ondisk(_handle, _systemConfigPath.toUtf8().constData(), (git_config_level_t)System, repository()->handle(), true);
            }

            if (_programDataConfigPath.isEmpty() == false) {
                git_config_add_file_ondisk(_handle, _programDataConfigPath.toUtf8().constData(), (git_config_level_t)ProgramData, repository()->handle(), true);
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

ConfigurationEntry Configuration::get(const QString& key)
{
    ConfigurationEntry result;

    git_config* snapshot = nullptr;
    git_config_entry* entry = nullptr;
    try
    {
        throwOnError(git_config_snapshot(&snapshot, _handle));
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
