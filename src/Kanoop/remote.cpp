#include "remote.h"

#include <credentialresolver.h>
#include <repository.h>
#include <referencecollection.h>

#include <Kanoop/commonexception.h>

using namespace GIT;

Remote::Remote(Repository* repo, git_remote* handle) :
    GitEntity(RemoteEntity, repo),
    _handle(handle)
{
    commonInit();
}

Remote::Remote(Repository* repo, const QString& name) :
    GitEntity(RemoteEntity, repo),
    _handle(nullptr)
{
    if(git_remote_lookup(&_handle, repo->handle(), name.toUtf8().constData()) != 0) {
        _handle = nullptr;
    }
    commonInit();
}

Remote::~Remote()
{
    if(_handle != nullptr) {
        git_remote_free(_handle);
    }
    if(_references != nullptr) {
        delete _references;
    }
}

void Remote::commonInit()
{
    if(_handle != nullptr) {
        _url = git_remote_url(_handle);
        _name = git_remote_name(_handle);
    }
    _references = new ReferenceCollection(repository());
}

void Remote::reloadReferences()
{
    delete _references;
    _references = new ReferenceCollection(repository());

    const git_remote_head** heads;
    size_t count;
    try
    {
        QMap<QString, Reference*> references;
        QMap<QString, QString> symRefs;

        git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
        callbacks.payload = repository();
        callbacks.credentials = repository()->credentialsCallback;
        throwOnError(git_remote_connect(_handle, GIT_DIRECTION_FETCH, &callbacks, nullptr, nullptr));
        throwOnError(git_remote_ls(&heads, &count, _handle));
        for(int i = 0;i < count;i++) {
            const git_remote_head* head = heads[i];
            QString name = head->name;
            QString symRefTargetName = head->symref_target;
            if(name.isEmpty()) {
                throw CommonException("Not expecting null value for reference name.");
            }

            if(symRefTargetName.isEmpty() == false) {
                symRefs.insert(name, symRefTargetName);
            }
            else {
                references.insert(name, new DirectReference(repository(), name, ObjectId(head->oid)));
            }
        }

        for(auto it = symRefs.begin();it != symRefs.end();it++) {
            QString key = it.key();
            QString value = it.value();
            if(references.contains(value) == false) {
                throw CommonException("Symbolic reference target not found in direct reference results.");
            }
            references.insert(key, new SymbolicReference(repository(), key, value, references.value(value)));
        }

        _references->append(references.values());
    }
    catch(const CommonException& e)
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
        throwOnError(git_remote_lookup(&remoteHandle, repository()->handle(), value.toUtf8().constData()));
        const git_refspec* refspec = git_remote_get_refspec(remoteHandle, 0);
        throwIfNull(refspec);
        throwOnError(git_refspec_rtransform(&buf, refspec, value.toUtf8().constData()));
        result = buf.ptr;
    }
    catch(const CommonException&)
    {
    }
    if(remoteHandle != nullptr) {
        git_remote_free(remoteHandle);
    }
    git_buf_free(&buf);
    return result;
}


