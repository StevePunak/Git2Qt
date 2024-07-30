#ifndef LIBGIT2OBJECT_H
#define LIBGIT2OBJECT_H

class LibGit2Object
{
protected:
    LibGit2Object(void* ptr) :
        _ptr(ptr) {}

public:
    virtual ~LibGit2Object();

private:
    void* _ptr;
};

#endif // LIBGIT2OBJECT_H
