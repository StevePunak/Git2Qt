#ifndef ANNOTATEDCOMMITHANDLE_H
#define ANNOTATEDCOMMITHANDLE_H
#include <git2.h>

namespace GIT {

class Repository;
class Branch;
class AnnotatedCommitHandle
{
public:
    AnnotatedCommitHandle(git_annotated_commit* handle);
    ~AnnotatedCommitHandle();

    static AnnotatedCommitHandle* fromRef(Repository* repo, Branch* branch);

    git_annotated_commit* handle() const { return _handle; }

private:
    git_annotated_commit* _handle = nullptr;
};

} // namespace

#endif // ANNOTATEDCOMMITHANDLE_H
