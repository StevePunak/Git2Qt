#include "graphbuilder.h"

#include "ancestorseeker.h"
#include "texttable.h"

#include <QElapsedTimer>
#include <gitexception.h>
#include <repository.h>
#include <stash.h>
#include <utility.h>

using namespace GIT;

GraphBuilder::GraphBuilder(Repository* repo) :
    GitEntity(GraphBuilderEntity, repo)
{
    _localBranches = repo->localBranches();
    _remoteBranches = repo->remoteBranches();
}

GraphBuilder::~GraphBuilder()
{
    qDeleteAll(_allCommits);
    _allCommits.clear();
}

bool GraphBuilder::calculateGraph()
{
    bool result = false;

    reset();

    try
    {
        Branch headBranch = repository()->head();
        if(headBranch.isNull()) {
            logText(LVL_ERROR, "Ref is null");
        }
        Commit headCommit = Commit::lookup(repository(), headBranch.reference().objectId());
        throwIfFalse(headCommit.isValid());

        // Create the list of all commits in time-order
        _allCommits = GraphBuilderCommit::PtrList(repository()->allCommits());

        // Create a map for quick lookup
        _commitIndex = GraphBuilderCommit::Map(_allCommits);

        // detect stashes and remove their unwanted children from the graph
        _allCommits.detectStashes(repository());

        // resolve commit relationships
        _allCommits.resolveParentsAndChildren();

        // resolve merges and stashes
        _allCommits.resolveMergesAndStashes(repository());

        // resolve branch names
        _allCommits.resolveBranchNames(this);

        // resolve the earliest commit for each branch
        buildBranchFromCommitIndex();

        // resolve merge births
        resolveMergeBirths();

        // Resolve the levels
        resolveGraphLevels();

        // Do the graphics stuff
        buildGraphLines();

        // create the result
        _graphedCommits = _allCommits.toGraphedCommitList();

        // clean up
        qDeleteAll(_allCommits);
        _allCommits.clear();

        result = true;
    }
    catch(const GitException&)
    {
        result = false;
    }

    return result;
}

Branch GraphBuilder::findBranchForReferencedObjectId(const ObjectId& objectId) const
{
    Branch branch;
    if((branch = _remoteBranches.findForReferencedObjectId(objectId)).isValid() == false) {
        branch = _localBranches.findForReferencedObjectId(objectId);
    }
    return branch;
}

GraphBuilderCommit* GraphBuilder::findCommit(const ObjectId& objectId) const
{
    return _commitIndex.value(objectId);
}

void GraphBuilder::dumpCommitTable(const GraphBuilderCommit::PtrList& table)
{
    QStringList headers = {
        "Message",
        "Branch",
        "Full Branch",
        "SHA",
        "Merge Base",
        "Merge From",
        "Merged Into",
        "Level",
        "Max Level",
        "Merge",
        "Stash",
        "Head",
        "Remote",
    };

    // make output object
    TextTable output(headers);

    for(const GraphBuilderCommit* commit : table) {
        QVector<QVariant> values;
        values.append(commit->shortMessage().trimmed().left(64));
        values.append(commit->friendlyBranchName());
        values.append(commit->branchName());
        values.append(commit->objectId().toString(6));
        values.append(commit->mergeBase().isValid() ? commit->mergeBase().toString(6) : "");
        values.append(commit->mergeFrom().isValid() ? commit->mergeFrom().toString(6) : "");
        values.append(commit->mergedInto().isValid() ? commit->mergedInto().toString(6) : "");
        values.append(commit->level());
        values.append(commit->maxLevel());
        values.append(Utility::toString(commit->isMerge()));
        values.append(Utility::toString(commit->isStash()));
        values.append(Utility::toString(commit->isHead()));
        values.append(Utility::toString(commit->isRemote()));
        for(int i = 0;i < values.count();i++)
            output.setValue(i, values[i]);
        output.nextRow();
    }

    QString result = output.toString();
    QTextStream(stdout) << result << Qt::endl;
}

void GraphBuilder::reset()
{
    qDeleteAll(_allCommits);
    _allCommits.clear();
    _commitIndex.clear();
    _mergeBirths.clear();
}

void GraphBuilder::resolveGraphLevels()
{
    for(int index = 0;index < _allCommits.count();index++) {
        GraphBuilderCommit* commit = _allCommits.at(index);

        // clean up levels that are no longer needed
        ObjectId::List mergeIds = _levelMap.mergeIds();
        for(const ObjectId& objectId : mergeIds) {
            GraphBuilderCommit* mergeBirth = _mergeBirths.value(objectId);
            if(mergeBirth == commit) {
                // blow away the old level
                int levelToRemove = _levelMap.levelForName(objectId.toString());
                QString mergeSourceString = GraphLevelMap::makeMergeSourceString(objectId);
                _levelMap.removeLevel(mergeSourceString);
                _levelMap.removeLevel(objectId.toString());
                _levelMap.removeLevel(levelToRemove);

                // If we are a merge at the same level we just blew away
                if(commit->isMerge() && commit->level() == levelToRemove && _levelMap.containsLevel(commit->level()) == false) {
                    _levelMap.addLevel(commit->objectId().toString(), commit->level());
                }
            }
        }

        QStringList branchBirths = _branchBirths.value(commit->objectId());
        for(const QString& branchName : branchBirths) {
            _levelMap.removeLevel(branchName);
        }


        // -------------------- BEGIN I think this is unreachable code
        GraphBuilderCommit::PtrList mergeChildren = _mergeParents[commit->objectId()];
        for(GraphBuilderCommit* mergeChild : mergeChildren) {
            if(_levelMap.containsName(mergeChild->objectId().toString())) {
                _levelMap.removeLevel(mergeChild->objectId().toString());
            }
        }
        // -------------------- END I think this is unreachable code

        if(commit->level() == 0) {
            int level = _levelMap.availableLevel();
            commit->setLevel(level);
            if(commit->isMerge()) {
                _levelMap.addLevel(commit->objectId().toString(), commit->level());
            }
            else {
                _levelMap.addLevel(commit->friendlyBranchName(), level);
            }
            resolveParentLevels(commit);
        }
        else {
            // set parent levels
            resolveParentLevels(commit);
        }
        commit->setMaxLevel(qMax(_levelMap.maxLevel(), 1));
    }
}

void GraphBuilder::resolveParentLevels(GraphBuilderCommit* commit)
{
if(commit->objectId().sha().startsWith("452b7")) {
    logText(LVL_DEBUG, "Ehere");
}
    if(commit->isMerge()) {
        // Merge base is on the same level as this commit
        GraphBuilderCommit* mergeBaseCommit = _commitIndex.value(commit->mergeBase());
        if(mergeBaseCommit->level() == 0) {
            mergeBaseCommit->setLevel(commit->level());
            _levelMap.addLevel(commit->objectId().toString(), commit->level());
            // if(_levelMap.containsLevel(commit->level()) == false) {
            //     _levelMap.addLevel(commit->objectId().toString(), commit->level());
            // }
        }

        // Merge from gets a new level
        GraphBuilderCommit* mergeFromCommit = _commitIndex.value(commit->mergeFrom());
        if(mergeFromCommit->level() == 0) {
            int level = _levelMap.availableLevel();
            if(mergeFromCommit->isMerge()) {
                _levelMap.addLevel(mergeFromCommit->objectId().toString(), level);
            }
            else {
                _levelMap.addLevel(GraphLevelMap::makeMergeSourceString(commit->objectId()), level);
            }
            mergeFromCommit->setLevel(level);
        }
    }
    else if(commit->parentCommitsRef().count() == 1) {
        // If only a single parent, we will probably use this commit level for the parent
        GraphBuilderCommit* parent = commit->parentCommitsRef().at(0);
        // set future single parents level
        while(parent->level() == 0) {
            parent->setLevel(commit->level());
            if(parent->parentCommitsRef().count() == 1 && parent->childCommitsRef().count() == 1) {
                parent = parent->parentCommitsRef().at(0);
                if(parent->childCommitsRef().count() > 1 || parent->parentCommitsRef().count() > 1) {
                    break;
                }
            }
            else {
                break;
            }
        }

#if ATTEMPT_2
            if(parent->childCommitsRef().count() < 3) {
                parent->setLevel(commit->level());
            }
            else {
                // In the case that there are more than two children of the parent,
                // Find the child of the parent (other than this commit) who has a level
                // Set and use that
                GraphBuilderCommit::PtrList parentChildren = parent->childCommitsRef();
                parentChildren.removeAll(commit);
                for(GraphBuilderCommit* child : parentChildren) {
                    if(child->level() != 0) {
                        parent->setLevel(child->level());
                        break;
                    }
                }

                // Didn't find any. Use this commit level.
                if(parent->level() == 0) {
                    parent->setLevel(commit->level());
                }
            }
#endif
    }
    else {
        for(GraphBuilderCommit* parent :  commit->parentCommitsRef()) {
            setCommitLevel(commit, parent);
        }
    }
}

int GraphBuilder::setCommitLevel(GraphBuilderCommit* parent, GraphBuilderCommit* child)
{
    Q_UNUSED(parent)

    int level = child->level();
    if(level != 0) {
        return level;
    }

    if(_levelMap.containsName(child->friendlyBranchName())) {
        level = _levelMap.levelForName(child->friendlyBranchName());
        child->setLevel(level);
    }
    else if(_branchBirths.contains(child->objectId()) == false) {
        level = _levelMap.availableLevel();
        _levelMap.addLevel(child->friendlyBranchName(), level);
        child->setLevel(level);
    }
    else {
        logText(LVL_ERROR, "UNHANDLED COMMIT LEVEL");
    }
    return level;
}

void GraphBuilder::buildBranchFromCommitIndex()
{
    QStringList processedBranches;

    _branchBirths.clear();
    _mergeBirths.clear();
    for(int index = _allCommits.count() - 1;index >= 0;index--) {
        // Detect Branch Birth
        GraphBuilderCommit* commit = _allCommits.at(index);
        if(commit->parentCommitsRef().count() == 0) {
            _branchBirths.insert(commit->objectId(), QStringList() << commit->friendlyBranchName());
            processedBranches.append(commit->friendlyBranchName());
        }

        for(GraphBuilderCommit* childCommit : commit->childCommitsRef()) {
            if(childCommit->friendlyBranchName() != commit->friendlyBranchName()) {
                if(processedBranches.contains(childCommit->friendlyBranchName()) == false) {
                    _branchBirths[commit->objectId()].append(childCommit->friendlyBranchName());
                    commit->addBranchBase(childCommit->friendlyBranchName());
                    processedBranches.append(childCommit->friendlyBranchName());
                }
            }
        }
    }
}

void GraphBuilder::resolveMergeBirths()
{
    for(GraphBuilderCommit* commit : _allCommits) {
        // Detect Merge Births
        if(commit->isMerge()) {
            MergeBaseSeeker seeker(commit, _commitIndex);
            if(seeker.birthCommit() != nullptr) {
                _mergeBirths.insert(commit->objectId(), seeker.birthCommit());
                _mergeParents[seeker.birthCommit()->objectId()].append(commit);
            }

        }
    }
}

void GraphBuilder::buildGraphLines()
{
    for(int index = 0;index < _allCommits.count();index++) {
        GraphBuilderCommit* commit = _allCommits.at(index);

if(commit->objectId().sha().startsWith("e067")) {
    logText(LVL_DEBUG, "Here");
}

        // Always a dot at level
        GraphLine& thisLine = commit->graphLineRef();
        if(commit->isMerge()) {
            thisLine.setGraphItem(commit->level(), MergeDot | VerticalDown);
        }
        else {
            thisLine.setGraphItem(commit->level(), CommitDot | VerticalDown);
        }

        // If a merge, we will also go down and left-or-right
        if(commit->isMerge()) {
            GraphBuilderCommit* mergeFromCommit = _commitIndex.value(commit->mergeFrom());
            drawMergeLine(commit, mergeFromCommit, mergeFromCommit->level());

            GraphBuilderCommit* mergeBaseCommit = _commitIndex.value(commit->mergeBase());
            drawMergeLine(commit, mergeBaseCommit, commit->level());
        }
        else {
            for(GraphBuilderCommit* parent : commit->parentCommitsRef()) {
                // draw verticals from this down to parent - 1
                for(int lineIndex = index + 1;lineIndex < parent->index();lineIndex++) {
                    GraphBuilderCommit* nextCommit = _allCommits.at(lineIndex);
                    int lineLevel = 0;
                    if(commit->isMerge()) {
                        lineLevel = parent->level();
                    }
                    else {
                        lineLevel = commit->level();
                    }
                    nextCommit->graphLineRef().setGraphItem(lineLevel, VerticalUp | VerticalDown);
                }

                // draw curves and horizontals at the parent
                if(commit->isMerge() == false) {
                    if(parent->level() == commit->level()) {
                        parent->graphLineRef().setGraphItem(commit->level(), VerticalUp);
                    }
                    else if(parent->level() < commit->level()) {
                        parent->graphLineRef().setGraphItem(commit->level(), DownToLeft);
                        if(parent->level() < commit->level() - 1) {
                            parent->graphLineRef().setGraphItem(parent->level(), HorizontalRight);
                        }

                        for(int level = parent->level() + 1;level < commit->level();level++) {
                            parent->graphLineRef().setGraphItem(level, HorizontalLeft);
                            if(level < commit->level() - 1) {
                                parent->graphLineRef().setGraphItem(level, HorizontalRight);
                            }
                        }
                    }
                    else if(parent->level() > commit->level()) {
                        parent->graphLineRef().setGraphItem(commit->level(), DownToRight);
                        if(commit->level() < parent->level() - 1) {
                            parent->graphLineRef().setGraphItem(parent->level(), HorizontalLeft);
                        }
                        for(int level = commit->level() + 1;level < parent->level();level++) {
                            parent->graphLineRef().setGraphItem(level, HorizontalRight);
                            if(level > commit->level() + 1) {
                                parent->graphLineRef().setGraphItem(level, HorizontalLeft);
                            }
                        }
                    }
                }
                else {
                    if(parent->level() == commit->level()) {
                        parent->graphLineRef().setGraphItem(commit->level(), VerticalUp);
                    }
                    else {
                        parent->graphLineRef().setGraphItem(parent->level(), VerticalUp);
                    }
                }
            }
        }
    }
}

GraphItemType GraphBuilder::drawMergeLine(GraphBuilderCommit* commit, GraphBuilderCommit* toCommit, int lineLevel)
{
    GraphItemType direction;

    // draw first row horizontal stuff
    if(lineLevel > commit->level()) {
        direction = RightThenDown;
        drawHorizontal(commit, commit->level(), lineLevel - 1);
        commit->graphLineRef().setGraphItem(lineLevel - 1, RightThenDown);
    }
    else if(lineLevel < commit->level()) {
        direction = LeftThenDown;
        drawHorizontal(commit, commit->level(), lineLevel + 1);
        commit->graphLineRef().setGraphItem(lineLevel + 1, LeftThenDown);
    }
    else {
        direction = VerticalDown;
        commit->graphLineRef().setGraphItem(commit->level(), VerticalDown);
    }

    // draw verticals
    for(int lineIndex = commit->index() + 1;lineIndex < toCommit->index();lineIndex++) {
        GraphBuilderCommit* nextCommit = _allCommits.at(lineIndex);
        nextCommit->graphLineRef().setGraphItem(lineLevel, VerticalUp | VerticalDown);
    }

    if(lineLevel != toCommit->level()) {
        // draw last row horizontal stuff
        if(toCommit->level() > commit->level()) {
            drawHorizontal(toCommit, commit->level() + 1, toCommit->level());
            toCommit->graphLineRef().setGraphItem(lineLevel, DownToRight);
        }
        else if(toCommit->level() < commit->level()) {
            drawHorizontal(toCommit, commit->level() - 1, toCommit->level());
            toCommit->graphLineRef().setGraphItem(lineLevel, DownToLeft);
        }
        else {
            toCommit->graphLineRef().setGraphItem(commit->level(), VerticalUp);
        }
    }
    else {
        toCommit->graphLineRef().setGraphItem(toCommit->level(), VerticalUp);
    }

    return direction;
}

void GraphBuilder::drawHorizontal(GraphBuilderCommit* commit, int fromLevel, int toLevel)
{
    int from = qMin(fromLevel, toLevel);
    int to = qMax(fromLevel, toLevel);

    if(to == from) {
        return;
    }

    // draw end half-lines
    commit->graphLineRef().setGraphItem(from, HorizontalRight);
    commit->graphLineRef().setGraphItem(to, HorizontalLeft);

    // draw horizontal as needed
    for(int level = from + 1;level < to;level++) {
        commit->graphLineRef().setGraphItem(level, HorizontalRight | HorizontalLeft);
    }
}

bool GraphBuilder::isMergeComplete(GraphBuilderCommit* mergeCommit, int atIndex) const
{
    bool complete = false;

    GraphBuilderCommit* birthCommit = _mergeBirths.value(mergeCommit->objectId());
    complete = birthCommit != nullptr && birthCommit->index() <= atIndex;

    return complete;
}

