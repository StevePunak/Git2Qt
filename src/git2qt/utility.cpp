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

bool Utility::readAllLines(const QString& filename, QStringList& lines)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)) {
        lines.clear();
        QTextStream ts(&file);
        QString line;
        while((line = ts.readLine()).isNull() == false) {
            lines.append(line);
        }
        return true;
    }
    return false;
}

bool Utility::writeAllLines(const QString& filename, const QStringList& lines)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)) {
        foreach(const QString& line, lines) {
            file.write(line.toLatin1());
            file.write("\n");
        }
        return true;
    }
    return false;
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
