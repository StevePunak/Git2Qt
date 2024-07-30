#include "similarityoptions.h"

using namespace GIT;

SimilarityOptions::SimilarityOptions() :
    _renameDetectionMode(Default),
    _whitespaceMode(IgnoreLeadingWhitespace),
    _renameThreshold(50),
    _renameFromRewriteThreshold(50),
    _copyThreshold(50),
    _breakRewriteThreshold(60),
    _renameLimit(200) {}

SimilarityOptions SimilarityOptions::none()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = None;
    return opts;
}

SimilarityOptions SimilarityOptions::renames()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = Renames;
    return opts;
}

SimilarityOptions SimilarityOptions::exact()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = Exact;
    return opts;
}

SimilarityOptions SimilarityOptions::copies()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = Copies;
    return opts;
}

SimilarityOptions SimilarityOptions::copiesHarder()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = CopiesHarder;
    return opts;
}

SimilarityOptions SimilarityOptions::defaultOptions()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = Default;
    return opts;
}

