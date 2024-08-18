/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class HAS-A collection of Submodule object and is used
 * by the repository to maintain its submodule information.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef SUBMODULECOLLECTION_H
#define SUBMODULECOLLECTION_H
#include <Kanoop/git/gitentity.h>
#include <Kanoop/git/remote.h>
#include <Kanoop/git/submodule.h>

namespace GIT {
class Repository;

class SubmoduleCollection : public GitEntity
{
public:
    SubmoduleCollection(Repository* repo);

    Submodule lookup(const QString& name);

    Submodule value(const QString& name);
    Submodule::Map values();

    virtual bool isNull() const override;

private:

    // Callbacks
    static int submoduleCallback(git_submodule *sm, const char *name, void *payload);

    Submodule::Map _loadingSubmodules;
};

} // namespace GIT

#endif // SUBMODULECOLLECTION_H
