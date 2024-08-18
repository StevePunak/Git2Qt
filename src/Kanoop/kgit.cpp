#include "kgit.h"

#include "credentialresolver.cpp"

#include <Kanoop/klog.h>
#include <Kanoop/pathutil.h>

bool KGit::_git2Initialized = false;

KGit::KGit()
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

void KGit::ensureInitialized()
{
    if(!_git2Initialized) {
        initializeLibGit2();
    }
}

void KGit::initializeLibGit2()
{
    if(_git2Initialized == false) {
        git_libgit2_init();
    }
    _git2Initialized = true;
}

