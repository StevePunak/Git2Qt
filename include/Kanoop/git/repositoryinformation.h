/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class contains information about the repository object.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef REPOSITORYINFORMATION_H
#define REPOSITORYINFORMATION_H
#include <Kanoop/git/gittypes.h>
#include <Kanoop/git/gitentity.h>

namespace GIT {

class RepositoryInformation : public GitEntity
{
public:
    RepositoryInformation(Repository* repo);

    QString path() const { return _path; }
    QString workingDirectory() const { return _workingDirectory; }
    bool isBare() const { return _isBare; }
    bool isShallow() const { return _isShallow; }
    bool isHeadDetached() const;
    bool isHeadUnborn() const;
    CurrentOperation::Op currentOperation() const;
    QString message() const;

    virtual bool isNull() const override { return false; }

private:
    QString _path;
    QString _workingDirectory;
    bool _isBare;
    bool _isShallow;
};

} // namespace GIT

#endif // REPOSITORYINFORMATION_H
