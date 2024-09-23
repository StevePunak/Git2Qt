#include "utility.h"

namespace GIT {

const QChar Utility::SEPERATOR       = '/';

QString Utility::combine(const QString& p1, const QString& p2, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

QString Utility::combine(const QString& p1, const QString& p2, const QString& p3, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2 + SEPERATOR + p3);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

QString Utility::combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2 + SEPERATOR + p3 + SEPERATOR + p4);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

QString Utility::combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2 + SEPERATOR + p3 + SEPERATOR + p4 + SEPERATOR + p5);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

QString Utility::combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2 + SEPERATOR + p3 + SEPERATOR + p4 + SEPERATOR + p5 + SEPERATOR + p6);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

QString Utility::combine(const QString& p1, const QString& p2, const QString& p3, const QString& p4, const QString& p5, const QString& p6, const QString& p7, bool dropLeadingSlash)
{
    QString result = QDir::cleanPath(p1 + SEPERATOR + p2 + SEPERATOR + p3 + SEPERATOR + p4 + SEPERATOR + p5 + SEPERATOR + p6 + SEPERATOR + p7);
    if(dropLeadingSlash && result.at(0) == SEPERATOR) {
        result = result.mid(1);
    }
    return result;
}

} // namespace GIT
