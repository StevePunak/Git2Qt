#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H
#include <git2qt/gitentity.h>
#include <git2qt/private/graphbuildercommit.h>
#include <git2qt/private/graphlevelmap.h>
#include <git2qt/branch.h>

namespace GIT {
class Repository;

class GraphBuilder : public GitEntity
{
public:
    GraphBuilder(Repository* repo);
    virtual ~GraphBuilder();

    bool calculateGraph();
    void ancestorTest(const ObjectId& commitId);

    GraphedCommit::List graphedCommits() const { return _graphedCommits; }
    Branch findBranchForReferencedObjectId(const ObjectId& objectId) const;
    Branch::Map& localBranches() { return _localBranches; }
    Branch::Map& remoteBranches() { return _remoteBranches; }
    GraphBuilderCommit* findCommit(const ObjectId& objectId) const;

    virtual bool isNull() const override { return repository() != nullptr; }

    void dumpCommitTable(const GraphBuilderCommit::PtrList& table);

private:
    void reset();
    void resolveGraphLevels();
    void resolveParentLevels(GraphBuilderCommit* commit);
    int setCommitLevel(GraphBuilderCommit* parent, GraphBuilderCommit* child);
    void buildBranchFromCommitIndex();
    void resolveMergeBirths();
    void buildGraphLines();
    GraphItemType drawMergeLine(GraphBuilderCommit* commit, GraphBuilderCommit* toCommit, int lineLevel);
    void drawHorizontal(GraphBuilderCommit* commit, int fromLevel, int toLevel);
    bool isMergeComplete(GraphBuilderCommit* mergeCommit, int atIndex) const;

    GraphBuilderCommit::PtrList _allCommits;
    GraphBuilderCommit::Map _commitIndex;
    GraphLevelMap _levelMap;
    QMap<ObjectId, QStringList> _branchBirths;
    QMap<ObjectId, GraphBuilderCommit*> _mergeBirths;
    QMap<ObjectId, GraphBuilderCommit::PtrList> _mergeParents;

    Branch::Map _localBranches;
    Branch::Map _remoteBranches;

    GraphedCommit::List _graphedCommits;
};

} // namespace GIT

#endif // GRAPHBUILDER_H
