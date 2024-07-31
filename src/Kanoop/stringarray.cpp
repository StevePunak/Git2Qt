#include "stringarray.h"

using namespace GIT;

StringArray::StringArray(const QStringList& values)
{
    _native = {0};
    _native.count = values.count();
    _native.strings = new char*[_native.count];
    for(int i = 0;i < _native.count;i++) {
        QByteArray utf8 = values.at(i).toUtf8();
        char* ptr = new char[utf8.length() + 1];
        memset(ptr, 0, utf8.length() + 1);
        memcpy(ptr, utf8.constData(), utf8.length());
        _native.strings[i] = ptr;
    }
}

StringArray::~StringArray()
{
    for(int i = 0;i < _native.count;i++) {
        delete _native.strings[i];
    }
    delete _native.strings;
}

char* StringArray::at(int index) const
{
    char* result = nullptr;
    if(index < _native.count) {
        result = _native.strings[index];
    }
    return result;
}
