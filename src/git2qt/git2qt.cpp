#include "git2qt.h"

#include "credentialresolver.cpp"

#include "log.h"

bool Git2Qt::_git2Initialized = false;

Git2Qt::Git2Qt()
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

void Git2Qt::ensureInitialized()
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

void Git2Qt::initializeLibGit2()
{
    if(_git2Initialized == false) {
        git_libgit2_init();
    }
    _git2Initialized = true;
}

