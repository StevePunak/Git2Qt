#include "signature.h"

using namespace GIT;

Signature::Signature(const git_signature* nativeSignature)
{
    _name = nativeSignature->name;
    _email = nativeSignature->email;
    _timestamp = QDateTime::fromSecsSinceEpoch((int64_t)nativeSignature->when.time, Qt::UTC);
}

Signature::~Signature()
{
    if(_native != nullptr) {
        git_signature_free(_native);
    }
}

const git_signature* Signature::toNative()
{
    if(_native != nullptr) {
        git_signature_free(_native);
    }
    git_signature_new(&_native, _name.toUtf8().constData(), _email.toUtf8().constData(), (git_time_t)_timestamp.toMSecsSinceEpoch(), 0);
    return _native;
}
