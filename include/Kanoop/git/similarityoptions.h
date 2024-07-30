#ifndef SIMILARITYOPTIONS_H
#define SIMILARITYOPTIONS_H

namespace GIT {

class SimilarityOptions
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
        Default,

        /// <summary>
        /// Attempt no rename or copy detection
        /// </summary>
        None,

        /// <summary>
        /// Detect exact renames and copies (compare SHA hashes only)
        /// </summary>
        Exact,

        /// <summary>
        /// Detect fuzzy renames (use similarity metric)
        /// </summary>
        Renames,

        /// <summary>
        /// Detect renames and copies
        /// </summary>
        Copies,

        /// <summary>
        /// Detect renames, and include unmodified files when looking for copies
        /// </summary>
        CopiesHarder,
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
