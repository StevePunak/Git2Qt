#include "log.h"

#include <QDateTime>
#include <QFileInfo>
#include <QString>
#include <QTextStream>

using namespace GIT;

const Log::LogLevelToStringMap Log::_LogLevelToStringMap;
QTextStream Log::_stdout(stdout);

void Log::logText(const char* file, int line, LogLevel level, const QString& text)
{
    QString result;
    QTextStream output(&result);

    QDateTime now = QDateTime::currentDateTimeUtc();
    QString formatted = QString().asprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d",
                                          now.date().year(), now.date().month(), now.date().day(),
                                          now.time().hour(), now.time().minute(), now.time().second(), now.time().msec());
    output << formatted << ' ';

    output << QFileInfo(file).fileName().leftJustified(12, ' ', true) << ' ';
    output << QString::number(line).leftJustified(4, ' ', true) << ' ';

    output << _LogLevelToStringMap.getString(level) << ' ';

    output << text;
    output << Qt::endl;

    QTextStream(stdout) << result;
}
