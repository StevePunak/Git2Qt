#include "remote.h"

#include <credentialresolver.h>
#include <repository.h>
#include <referencecollection.h>
#include <gitexception.h>

#include "log.h"

using namespace GIT;

Remote::Remote(Repository* repo, const QString& name) :
    GitEntity(RemoteEntity, repo),
    _name(name)
{
    commonInit();
}

Remote::~Remote()
{
    if(_references != nullptr) {
        delete _references;
    }
}

void Remote::dispose()
{
    createHandle().dispose();
}

void Remote::commonInit()
{
    RemoteHandle handle = createHandle();
    if(handle.isNull() == false) {
        _url = git_remote_url(handle.value());
        _name = git_remote_name(handle.value());
        handle.dispose();
    }
    _references = new ReferenceCollection(repository());
}

RemoteHandle Remote::createHandle() const
{
    RemoteHandle handle;
    git_remote* remote = nullptr;
    if(git_remote_lookup(&remote, repository()->handle().value(), _name.toUtf8().constData()) == 0) {
        handle = RemoteHandle(remote);
    }
    return handle;
}

void Remote::reloadReferences()
{
    delete _references;
    _references = new ReferenceCollection(repository());

    const git_remote_head** heads;
    size_t count;
    try
    {
        {
            git_strarray ref_names;
            throwOnError(git_reference_list(&ref_names, repository()->handle().value()));
            for(int i = 0;i < (int)ref_names.count;i++) {
                git_reference* ref = nullptr;
                const char* name = ref_names.strings[i];
                int res = git_reference_lookup(&ref, repository()->handle().value(), name);
                Log::logText(LVL_DEBUG, QString("[%1] Res: %2").arg(name).arg(res));

                git_reference_free(ref);
            }

        }



        QMap<QString, Reference> references;
        QMap<QString, QString> symRefs;

        RemoteHandle handle = createHandle();
        if(handle.isNull()) {
            throw GitException("Failed to create handle for reference");
        }

        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        callbacks.payload = repository();
        callbacks.credentials = repository()->credentialsCallback;
        throwOnError(git_remote_connect(handle.value(), GIT_DIRECTION_FETCH, &callbacks, nullptr, nullptr));
        throwOnError(git_remote_ls(&heads, &count, handle.value()));
        for(int i = 0;i < (int)count;i++) {
            const git_remote_head* head = heads[i];
            QString name = head->name;
            QString symRefTargetName = head->symref_target;
            if(name.isEmpty()) {
                throw GitException("Not expecting null value for reference name.");
            }

            if(symRefTargetName.isEmpty() == false) {
                symRefs.insert(name, symRefTargetName);
            }
            else {
                references.insert(name, Reference::createDirectReferenceObject(repository(), name, ObjectId(head->oid)));
            }
        }

        for(auto it = symRefs.begin();it != symRefs.end();it++) {
            QString key = it.key();
            QString value = it.value();
            if(references.contains(value) == false) {
                throw GitException("Symbolic reference target not found in direct reference results.");
            }
            references.insert(key, Reference::createSymbolicReferenceObject(repository(), key, value));
        }

        _references->appendDirectReference(references.values());
    }
    catch(const GitException& e)
    {
    }
}

QString Remote::fetchSpecTransformToSource(const QString& value)
{
    QString result;

    git_remote* remoteHandle = nullptr;
    git_buf buf = GIT_BUF_INIT;
    try
    {
        throwOnError(git_remote_lookup(&remoteHandle, repository()->handle().value(), value.toUtf8().constData()));
        const git_refspec* refspec = git_remote_get_refspec(remoteHandle, 0);
        throwIfNull(refspec);
        throwOnError(git_refspec_rtransform(&buf, refspec, value.toUtf8().constData()));
        result = buf.ptr;
    }
    catch(const GitException&)
    {
    }
    if(remoteHandle != nullptr) {
        git_remote_free(remoteHandle);
    }
    git_buf_free(&buf);
    return result;
}


