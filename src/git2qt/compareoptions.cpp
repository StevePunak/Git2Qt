#include "compareoptions.h"

using namespace GIT;

CompareOptions::CompareOptions() :
    _contextLines(3),
    _interhunkLines(0),
    _includeUnmodified(false),
    _algorithm(Myers),
    _identHeurisitc(false)
{}
