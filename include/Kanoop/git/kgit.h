/**
 *  KGit
 *
 *  Stephen Punak, July 24 2024
 */
#ifndef KGIT_H
#define KGIT_H
#include <Kanoop/git/kanoopgit.h>
#include <QObject>
#include <git2.h>
#include <Kanoop/git/branch.h>
#include <Kanoop/git/compareoptions.h>
#include <Kanoop/git/configuration.h>
#include <Kanoop/git/diff.h>
#include <Kanoop/git/network.h>
#include <Kanoop/git/remote.h>
#include <Kanoop/git/remotecollection.h>
#include <Kanoop/git/repository.h>
#include <Kanoop/git/submodulecollection.h>
#include <Kanoop/git/tagcollection.h>

class CredentialResolver;
class KANOOPGIT_EXPORT KGit : public QObject
{
    Q_OBJECT
public:
    KGit();

    static void ensureInitialized();

private:
    static void initializeLibGit2();

    static bool _git2Initialized;
};

#endif // KGIT_H
