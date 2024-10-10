/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class is a port of the SimilarityOptions class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef SIMILARITYOPTIONS_H
#define SIMILARITYOPTIONS_H
#include <git2.h>
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT SimilarityOptions
{
public:
    SimilarityOptions();

    /// <summary>
    /// Represents a mode for handling whitespace while detecting renames and copies.
    /// </summary>
    enum WhitespaceMode
    {
        /// <summary>
        /// Don't consider leading whitespace when comparing files
        /// </summary>
        IgnoreLeadingWhitespace,

        /// <summary>
        /// Don't consider any whitespace when comparing files
        /// </summary>
        IgnoreAllWhitespace,

        /// <summary>
        /// Include all whitespace when comparing files
        /// </summary>
        DontIgnoreWhitespace,
    };

    /// <summary>
    /// Represents a mode for detecting renames and copies.
    /// </summary>
    enum RenameDetectionMode
    {
        /// <summary>
        /// Obey the user's `diff.renames` configuration setting
        /// </summary>
        RenameDetectionDefault,

        /// <summary>
        /// Attempt no rename or copy detection
        /// </summary>
        RenameDetectionNone,

        /// <summary>
        /// Detect exact renames and copies (compare SHA hashes only)
        /// </summary>
        RenameDetectionExact,

        /// <summary>
        /// Detect fuzzy renames (use similarity metric)
        /// </summary>
        RenameDetectionRenames,

        /// <summary>
        /// Detect renames and copies
        /// </summary>
        RenameDetectionCopies,

        /// <summary>
        /// Detect renames, and include unmodified files when looking for copies
        /// </summary>
        RenameDetectionCopiesHarder,
    };

    static SimilarityOptions none();
    static SimilarityOptions renames();
    static SimilarityOptions exact();
    static SimilarityOptions copies();
    static SimilarityOptions copiesHarder();
    static SimilarityOptions defaultOptions();

    RenameDetectionMode renameDetectionMode() const { return _renameDetectionMode; }
    void setRenameDetectionMode(RenameDetectionMode value) { _renameDetectionMode = value; }

    WhitespaceMode whitespaceMode() const { return _whitespaceMode; }
    void setWhitespaceMode(WhitespaceMode value) { _whitespaceMode = value; }

    int renameThreshold() const { return _renameThreshold; }
    void setRenameThreshold(int value) { _renameThreshold = value; }

    int renameFromRewriteThreshold() const { return _renameFromRewriteThreshold; }
    void setRenameFromRewriteThreshold(int value) { _renameFromRewriteThreshold = value; }

    int copyThreshold() const { return _copyThreshold; }
    void setCopyThreshold(int value) { _copyThreshold = value; }

    int breakRewriteThreshold() const { return _breakRewriteThreshold; }
    void setBreakRewriteThreshold(int value) { _breakRewriteThreshold = value; }

    int renameLimit() const { return _renameLimit; }
    void setRenameLimit(int value) { _renameLimit = value; }

    git_diff_find_options toNativeDiffFindOptions() const;

private:
    /// <summary>
    /// The mode for detecting renames and copies
    /// </summary>
    RenameDetectionMode _renameDetectionMode;

    /// <summary>
    /// The mode for handling whitespace when comparing files
    /// </summary>
    WhitespaceMode _whitespaceMode;

    /// <summary>
    /// Similarity in order to consider a rename
    /// </summary>
    int _renameThreshold;

    /// <summary>
    /// Similarity of a modified file in order to be eligible as a rename source
    /// </summary>
    int _renameFromRewriteThreshold;

    /// <summary>
    /// Similarity to consider a file a copy
    /// </summary>
    int _copyThreshold;

    /// <summary>
    /// Similarity to split modify into an add/delete pair
    /// </summary>
    int _breakRewriteThreshold;

    /// <summary>
    /// Maximum similarity sources to examine for a file
    /// </summary>
    int _renameLimit;
};

} // namespace GIT

#endif // SIMILARITYOPTIONS_H
