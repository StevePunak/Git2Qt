/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class provides the available diff compare options.
 * It is a port of the CompareOptions class from libgit2sharp.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef COMPAREOPTIONS_H
#define COMPAREOPTIONS_H

#include <Kanoop/git/similarityoptions.h>
#include <Kanoop/git/gittypes.h>

namespace GIT {

class CompareOptions
{
public:
    CompareOptions();

    int contextLines() const { return _contextLines; }
    void setContextLines(int value) { _contextLines = value; }

    int interhunkLines() const { return _interhunkLines; }
    void setInterhunkLines(int value) { _interhunkLines = value; }

    SimilarityOptions similarity() const { return _similarity; }
    void setSimilarity(SimilarityOptions value) { _similarity = value; }

    bool includeUnmodified() const { return _includeUnmodified; }
    void setIncludeUnmodified(bool value) { _includeUnmodified = value; }

    DiffAlgorithm algorithm() const { return _algorithm; }
    void setAlgorithm(DiffAlgorithm value) { _algorithm = value; }

    bool identHeurisitc() const { return _identHeurisitc; }
    void setIdentHeurisitc(bool value) { _identHeurisitc = value; }

private:
    int _contextLines = 3;
    int _interhunkLines = 0;
    SimilarityOptions _similarity;
    bool _includeUnmodified;
    DiffAlgorithm _algorithm;
    bool _identHeurisitc;
};

} // namespace GIT

#endif // COMPAREOPTIONS_H
