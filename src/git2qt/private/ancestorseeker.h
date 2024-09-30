#ifndef ANCESTORSEEKER_H
#define ANCESTORSEEKER_H
#include <git2qt/private/graphbuildercommit.h>

namespace GIT {

class MergeBaseSeeker
{
public:
    MergeBaseSeeker(GraphBuilderCommit* mergeCommit, const GraphBuilderCommit::Map &commitIndex);

    GraphBuilderCommit* birthCommit() const { return _birthCommit; }

private:
    void resolve1();
    void resolve2();

    GraphBuilderCommit* _mergeCommit;
    const GraphBuilderCommit::Map& _commitIndex;
    GraphBuilderCommit* _birthCommit;
};

class MergeTip
{
public:
    MergeTip(GraphBuilderCommit* head) :
        _head(head) {}

    GraphBuilderCommit* head() const { return _head; }

private:
    GraphBuilderCommit* _head;
    GraphBuilderCommit::PtrList _parents;
    GraphBuilderCommit::PtrList _newParents;
};

class MergeTipIndex : public QMap<ObjectId, MergeTip*>
{
public:

};

class AncestorSeeker
{
public:
    AncestorSeeker(GraphBuilderCommit* commit);

    GraphBuilderCommit* commit() const { return _commit; }

    void produceAncestors(int count);
    int minLevel() const;

    ObjectId::Set& ancestors() { return _ancestors; }
    QDateTime oldestAncestor() const { return _oldestAncestor; }

private:
    GraphBuilderCommit* _commit;
    GraphBuilderCommit::Set _tips;
    ObjectId::Set _ancestors;
    QDateTime _oldestAncestor;
};

class SeekerSet : public QList<AncestorSeeker*>
{
public:
    void produceAncestors();
};

} // namespace GIT

#endif // ANCESTORSEEKER_H
