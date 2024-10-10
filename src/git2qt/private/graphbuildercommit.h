#ifndef GRAPHBUILDERCOMMIT_H
#define GRAPHBUILDERCOMMIT_H
#include <git2qt/graphedcommit.h>

namespace GIT {

class GraphBuilder;

class GraphBuilderCommit : public GraphedCommit
{
public:
    GraphBuilderCommit();
    GraphBuilderCommit(const Commit& other);
    virtual ~GraphBuilderCommit();

    void copyBranchInformation(const GraphBuilderCommit* from);

    GraphedCommit toGraphedCommit() const;

    class PtrList : public QList<GraphBuilderCommit*>
    {
    public:
        PtrList() {}
        PtrList(const Commit::List& other) {
            for(const Commit& commit : other) {
                append(new GraphBuilderCommit(commit));
            }
        }

        void detectStashes(Repository* repo);
        void resolveParentsAndChildren();
        void resolveMergesAndStashes(Repository* repo);
        void resolveBranchNames(GraphBuilder* graphBuilder);

        GraphedCommit::List toGraphedCommitList() const
        {
            GraphedCommit::List result;
            for(GraphBuilderCommit* commit : *this) {
                result.append(commit->toGraphedCommit());
            }
            return result;
        }

        ObjectId::List objectIds() const
        {
            ObjectId::List result;
            for(GraphBuilderCommit* commit : *this) {
                result.append(commit->objectId());
            }
            return result;
        }
    };

    class Set : public QSet<GraphBuilderCommit*>
    {

    };

    class Map : public QMap<ObjectId, GraphBuilderCommit*>
    {
    public:
        Map() {}
        Map(PtrList& other) {
            for(GraphBuilderCommit* o : other) {
                insert(o->objectId(), o);
            }
        }
    };

    PtrList& parentCommitsRef() { return _parentCommits; }
    PtrList& childCommitsRef() { return _childCommits; }

private:
    PtrList _parentCommits;
    PtrList _childCommits;
};

} // namespace GIT

#endif // GRAPHBUILDERCOMMIT_H
