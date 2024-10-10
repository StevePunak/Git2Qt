#ifndef MERGEANALYSISRESULT_H
#define MERGEANALYSISRESULT_H
#include <git2qt/gittypes.h>

namespace GIT {

class MergeAnalysisResult
{
public:
    MergeAnalysisResult() :
        _analyses(MergeAnalysisNone),
        _preferences(MergePreferenceNone),
        _valid(false) {}

    MergeAnalysisResult(MergeAnalyses analyses, MergePreferences preferences) :
        _analyses(analyses),
        _preferences(preferences),
    _valid(true) {}

    MergeAnalyses analysis() const { return _analyses; }
    MergePreferences preference() const { return _preferences; }

    bool isValid() const { return _valid; }

private:
    MergeAnalyses _analyses = MergeAnalysisNone;
    MergePreferences _preferences = MergePreferenceNone;
    bool _valid = false;
};

} // namespace GIT

#endif // MERGEANALYSISRESULT_H
