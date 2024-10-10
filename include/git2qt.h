/**
 * Copyright (c) 2024 Stephen Punak
 *
 * Master header for libgit2qt
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef GIT2QT_H
#define GIT2QT_H
#include <QObject>
#include <git2qt/kanoopgit.h>
#include <git2.h>
#include <git2qt/branchcollection.h>
#include <git2qt/commitlog.h>
#include <git2qt/compareoptions.h>
#include <git2qt/configuration.h>
#include <git2qt/diff.h>
#include <git2qt/network.h>
#include <git2qt/remote.h>
#include <git2qt/remotecollection.h>
#include <git2qt/repository.h>
#include <git2qt/tagcollection.h>
#include <git2qt/annotatedtag.h>
#include <git2qt/lightweighttag.h>
#include <git2qt/referencecollection.h>
#include <git2qt/repositorystatus.h>
#include <git2qt/index.h>
#include <git2qt/tree.h>
#include <git2qt/treechanges.h>
#include <git2qt/commands.h>
#include <git2qt/abstractcredentialresolver.h>
#include <git2qt/progresscallback.h>
#include <git2qt/declspec.h>

class CredentialResolver;
class GIT2QT_EXPORT Git2Qt : public QObject
{
    Q_OBJECT
public:
    Git2Qt();

    static void ensureInitialized();

private:
    static void initializeLibGit2();

    static bool _git2Initialized;
};

#endif // GIT2QT_H
