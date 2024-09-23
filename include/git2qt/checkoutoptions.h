/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_checkout_options from libgit2.
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef CHECKOUTOPTIONS_H
#define CHECKOUTOPTIONS_H
#include <QStringList>
#include <git2.h>
#include <git2qt/stringarray.h>

namespace GIT {

class CheckoutOptions
{
public:
    enum CheckoutModifiers
    {
        /// <summary>
        /// No checkout flags - use default behavior.
        /// </summary>
        None = 0,

        /// <summary>
        /// Proceed with checkout even if the index or the working tree differs from HEAD.
        /// This will throw away local changes.
        /// </summary>
        Force,
    };

    CheckoutOptions() :
        _modifiers(None) {}

    CheckoutModifiers modifiers() const { return _modifiers; }
    void setModifiers(CheckoutModifiers value) { _modifiers = value; }

    QStringList paths() const { return _paths; }
    void setPaths(QStringList value) { _paths = value; }

    const git_checkout_options* toNative();

private:
    CheckoutModifiers _modifiers;

    QStringList _paths;
    StringArray _pathsArray;

    git_checkout_options _options;
};

} // namespace GIT

#endif // CHECKOUTOPTIONS_H
