#include "stringarray.h"

using namespace GIT;

StringArray::StringArray(const QStringList& values)
{
    _count = values.count();
    _values = new char*[_count];
    for(int i = 0;i < _count;i++) {
        QByteArray utf8 = values.at(i).toUtf8();
        char* ptr = new char[utf8.length() + 1];
        memset(ptr, 0, utf8.length() + 1);
        memcpy(ptr, utf8.constData(), utf8.length());
        _values[i] = ptr;
    }
}

StringArray::~StringArray()
{
    for(int i = 0;i < _count;i++) {
        delete _values[i];
    }
    delete _values;
}

char* StringArray::at(int index) const
{
    char* result = nullptr;
    if(index < _count) {
        result = _values[index];
    }
    return result;
}
