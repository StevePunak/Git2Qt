/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class contains some utility objects copied directly from the StringUtil class
 * in KanoopCommonQt library to remove dependency.
 *
 * The original exists and is maintained at:
 *
 *   git@github.com:StevePunak/KanoopCommonQt.git
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef UTILITY_H
#define UTILITY_H

#include <QDir>
#include <QString>
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT Utility
{
public:
    static QString toString(bool value) { return value ? "true" : "false"; }
    static QString toString(const QDateTime& date) { return date.toString("yyyy-MM-ddThh:mm:ss.zzzZ"); }

    /**
     * @brief combine paths into a normalized path
     */
    static QString combine(const QString& p1, const QString& p2, bool dropLeadingSlash = false);
    static QString combine(const QString& p1, const QString& p2, const QString& p3, bool dropLeadingSlash = false);
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, bool dropLeadingSlash = false);
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, bool dropLeadingSlash = false);
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6, bool dropLeadingSlash = false);
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6, const QString& p7, bool dropLeadingSlash = false);
    static QString combine(const QStringList& parts);

    static bool readAllLines(const QString& filename, QStringList& lines);
    static bool writeAllLines(const QString& filename, const QStringList& lines);

private:
    static const QChar SEPERATOR;
};

} // namespace GIT

#endif // UTILITY_H
