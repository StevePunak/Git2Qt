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

namespace GIT {

class Utility
{
public:
    static QString toString(bool value) { return value ? "true" : "false"; }
    static QString toString(const QDateTime& date) { return date.toString("yyyy-MM-ddThh:mm:ss.zzzZ"); }

    /**
     * @brief combine paths into a normalized path
     */
    static QString combine(const QString& p1, const QString& p2)
    { return QDir::cleanPath(p1 + QDir::separator() + p2); }
    static QString combine(const QString& p1, const QString& p2, const QString& p3)
    { return QDir::cleanPath(p1 + QDir::separator() + p2 + QDir::separator() + p3); }
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4)
    { return QDir::cleanPath(p1 + QDir::separator() + p2 + QDir::separator() + p3 + QDir::separator() + p4); }
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5)
    { return QDir::cleanPath(p1 + QDir::separator() + p2 + QDir::separator() + p3 + QDir::separator() + p4 + QDir::separator() + p5); }
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6)
    { return QDir::cleanPath(p1 + QDir::separator() + p2 + QDir::separator() + p3 + QDir::separator() + p4 + QDir::separator() + p5 + QDir::separator() + p6); }
    static QString combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6, const QString& p7)
    { return QDir::cleanPath(p1 + QDir::separator() + p2 + QDir::separator() + p3 + QDir::separator() + p4 + QDir::separator() + p5 + QDir::separator() + p6 + QDir::separator() + p7); }
    static QString combine(const QStringList& parts);
};

} // namespace GIT

#endif // UTILITY_H
