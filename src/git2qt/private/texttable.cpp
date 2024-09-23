#include "texttable.h"
#include <git2qt/log.h>
#include <QTextStream>

using namespace GIT;

void TextTable::nextRow()
{
    _rows.append(_currentRow);
    createEmptyRow();
}

void TextTable::setValue(const QString &header, const QVariant &value)
{
    int index = _headers.indexOf(header);
    if(index < 0)
        return;
    setValue(index, value.toString());
}

void TextTable::setValue(int index, const QVariant &value)
{
    if(index >= _headers.count())
        return;

    _currentRow.setValue(index, value.toString());
}

void TextTable::addSubValue(const QString &header, const QVariant &value)
{
    int index = _headers.indexOf(header);
    if(index < 0)
        return;
    addSubValue(index, value.toString());
}

void TextTable::addSubValue(int index, const QVariant &value)
{
    if(index >= _headers.count())
        return;

    _currentRow.addSubValue(index, value.toString());
}

QString TextTable::toString() const
{
    QString outputString;
    outputString.reserve(4096);
    QTextStream output(&outputString);

    // get col widths
    QVector<int> colWidths;
    QVector<int> colPositions;
    int colPos = 0;
    for(int i = 0;i < _headers.count();i++)
    {
        colWidths.append(getColumnWidth(i) + 2);
        colPositions.append(colPos);
        colPos += colWidths.last();
    }

    // make headers
    for(int i = 0;i < _headers.count();i++)
    {
        if(i < _headers.count() - 1)
            output << _headers[i].leftJustified(colWidths[i]);
        else
            output << _headers[i];
    }
    output << Qt::endl;

    // make separators
    for(int i = 0;i < _headers.count();i++)
    {
        if(i < _headers.count() - 1)
            output << QString().leftJustified(_headers[i].length(), '=').leftJustified(colWidths[i]);
        else
            output << QString().leftJustified(_headers[i].length(), '=');
    }
    output << Qt::endl;

    // spit out each row
    foreach(const Row& row, _rows)
    {
        QStringList values = row.values();
        for(int i = 0;i < values.length();i++)
        {
            QString contents;
            if(i < _headers.count() - 1)
                contents = values[i].leftJustified(colWidths[i]);
            else
                contents = values[i];
            output << contents;
        }
        output << Qt::endl;

        QMap<int, QStringList> subValueMap = row.subValues();
        if(subValueMap.count() > 0) {
            QList<int> keys = subValueMap.keys();
            std::sort(keys.begin(), keys.end(), [](const int& a, const int& b) {
                   return a < b;
               });
            for(int col : keys) {
                if(col >= colWidths.length()) {
                    Log::logText(LVL_WARNING, "Invalid subvalue in table");
                    continue;
                }

                QStringList subValues = subValueMap.value(col);
                for(int i = 0;i < subValues.count();i++) {
                    QString contents = QString().leftJustified(colPositions[col]);
                    contents += subValues[i];
                    output << contents << Qt::endl;
                }
            }
        }
    }
    return outputString;
}

void TextTable::createEmptyRow()
{
    _currentRow.clear();
    while(_currentRow.values().count() < _headers.count())
        _currentRow.append("");
}

int TextTable::getColumnWidth(int index) const
{
    int max = _headers[index].length();
    foreach(const Row& row, _rows)
    {
        QStringList values = row.values();
        if(values.count() <= index)
            continue;
        if(values[index].length() > max)
            max = values[index].length();
    }
    return max;
}
