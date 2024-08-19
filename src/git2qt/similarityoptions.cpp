#include "similarityoptions.h"
#include <gittypes.h>

using namespace GIT;

SimilarityOptions::SimilarityOptions() :
    _renameDetectionMode(RenameDetectionDefault),
    _whitespaceMode(IgnoreLeadingWhitespace),
    _renameThreshold(50),
    _renameFromRewriteThreshold(50),
    _copyThreshold(50),
    _breakRewriteThreshold(60),
    _renameLimit(200) {}

SimilarityOptions SimilarityOptions::none()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionNone;
    return opts;
}

SimilarityOptions SimilarityOptions::renames()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionRenames;
    return opts;
}

SimilarityOptions SimilarityOptions::exact()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionExact;
    return opts;
}

SimilarityOptions SimilarityOptions::copies()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionCopies;
    return opts;
}

SimilarityOptions SimilarityOptions::copiesHarder()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionCopiesHarder;
    return opts;
}

SimilarityOptions SimilarityOptions::defaultOptions()
{
    SimilarityOptions opts;
    opts._renameDetectionMode = RenameDetectionDefault;
    return opts;
}

git_diff_find_options SimilarityOptions::toNativeDiffFindOptions() const
{
    git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;

    opts.rename_threshold = _renameThreshold;
    opts.rename_from_rewrite_threshold = _renameFromRewriteThreshold;
    opts.copy_threshold = _copyThreshold;
    opts.break_rewrite_threshold = _breakRewriteThreshold;
    opts.rename_limit = _renameLimit;

    switch(_renameDetectionMode) {
    case RenameDetectionExact:
        opts.flags = DiffFindExactMatchOnly |
                     DiffFindRenames |
                     DiffFindCopies |
                     DiffFindCopiesFromUnmodified;
        break;
    case RenameDetectionRenames:
        opts.flags = DiffFindRenames;
        break;
    case RenameDetectionCopies:
        opts.flags = DiffFindRenames |
                     DiffFindCopies;
        break;
    case RenameDetectionCopiesHarder:
        opts.flags = DiffFindRenames |
                     DiffFindCopies |
                     DiffFindCopiesFromUnmodified;
        break;
    default:
        break;
    }

    switch(_whitespaceMode) {
    case DontIgnoreWhitespace:
        opts.flags |= DiffFindDontIgnoreWhitespace;
        break;
    case IgnoreLeadingWhitespace:
        opts.flags |= DiffFindIgnoreLeadingWhitespace;
        break;
    case IgnoreAllWhitespace:
        opts.flags |= DiffFindIgnoreWhitespace;
        break;
    default:
        break;
    }

    return opts;
}

