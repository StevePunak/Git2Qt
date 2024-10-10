#include "remote.h"

#include <abstractcredentialresolver.h>
#include <repository.h>
#include <referencecollection.h>
#include <gitexception.h>
#include <utility.h>

#include "log.h"

using namespace GIT;

Remote::Remote() :
    GitEntity(RemoteEntity, nullptr),
    _references(nullptr)
{
}

Remote::Remote(Repository* repo, const QString& name) :
    GitEntity(RemoteEntity, repo),
    _name(name),
    _references(repo)
{
    commonInit();
}

Remote::~Remote()
{
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
    _references.clear();

    try
    {
        QMap<QString, Reference> references;
        QMap<QString, QString> symRefs;

        git_strarray ref_names;
        throwOnError(git_reference_list(&ref_names, repository()->handle().value()));

        // First pass. Resolve all direct references and build a list of symbolic references
        for(int i = 0;i < (int)ref_names.count;i++) {
            git_reference* ref = nullptr;
            const char* name = ref_names.strings[i];
            throwOnError(git_reference_lookup(&ref, repository()->handle().value(), name));

            ReferenceType type = (ReferenceType)git_reference_type(ref);
            switch(type) {
            case DirectReferenceType:
            {
                const git_oid* oid = git_reference_target(ref);
                throwIfNull(oid);
                references.insert(name, Reference::createDirectReferenceObject(repository(), name, ObjectId(oid)));
                break;
            }
            case SymbolicReferenceType:
            {
                const char* targetName = git_reference_symbolic_target(ref);
                throwIfNull(targetName, "Bad return from git_reference_symbolic_target");
                symRefs.insert(name, targetName);
                break;
            }
            default:
                Log::logText(LVL_ERROR, "Unhandled reference type");
                break;
            }


            git_reference_free(ref);
        }

        // Resolve symbolic references in this second pass
        for(auto it = symRefs.begin();it != symRefs.end();it++) {
            QString key = it.key();
            QString value = it.value();
            if(references.contains(value) == false) {
                throw GitException("Symbolic reference target not found in direct reference results.");
            }
            references.insert(key, Reference::createSymbolicReferenceObject(repository(), key, value));
        }

        _references.appendDirectReferences(references.values());
    }
    catch(const GitException&)
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

/**
 * @brief Remote::reloadReferencesFromHeads
 *
 * This code is the direct port from libgit2sharp. I'm keeping around for
 * reference, but I believe the correct way is in reloadReferences()
 *
 * This code should not be called until I understand why the c# project
 * uses this approach.
 */
void Remote::reloadReferencesFromHeads()
{
    _references.clear();

    const git_remote_head** heads;
    size_t count;
    try
    {
        QMap<QString, Reference> references;
        QMap<QString, QString> symRefs;

        RemoteHandle handle = createHandle();
        if(handle.isNull()) {
            throw GitException("Failed to create handle for remote");
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

        _references.appendDirectReferences(references.values());
    }
    catch(const GitException&)
    {
    }

}


