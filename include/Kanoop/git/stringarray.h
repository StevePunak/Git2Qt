#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <QStringList>


namespace GIT {

class StringArray
{
public:
    StringArray(const QStringList& values);
    virtual ~StringArray();

    char* at(int index) const;

private:
    char** _values;
    int _count;
};

} // namespace

#endif // STRINGARRAY_H
