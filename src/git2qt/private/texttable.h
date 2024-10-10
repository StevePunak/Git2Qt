#ifndef TEXTTABLE_H
#define TEXTTABLE_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

class TextTable
{
public:
    TextTable(const QStringList& headers) :
        _headers(headers)
    {
        createEmptyRow();
    }

    /**
     * @brief nextRow
     * Add a new row to the repsonse
     */
    void nextRow();

    /**
     * @brief setValue
     * Set the value for the current row and column
     * @param header Which column to set the value for
     * @param value The value to set
     */
    void setValue(const QString& header, const QVariant& value);

    /**
     * @brief setValue
     * Set the value for the current row and column
     * @param index Which column to set the value for
     * @param value The value to set
     */
    void setValue(int index, const QVariant &value);

    /**
     * @brief addSubValue
     * Set a sub-value for the given column
     * @param index
     * @param value
     */
    void addSubValue(const QString& header, const QVariant& value);

    /**
     * @brief addSubValue
     * Set a sub-value for the given column
     * @param index
     * @param value
     */
    void addSubValue(int index, const QVariant& value);

    /**
     * @brief toString
     * Get the formatted output for return to the terminal
     * @return
     */
    QString toString() const;

private:
    class Row
    {
    public:
        Row() {}

        void clear()
        {
            _values.clear();
            _subValues.clear();
        }

        void append(const QString& value)
        {
            _values.append(value);
        }

        void setValue(int index, const QString& value)
        {
            if(index >= _values.count()) {
                _values.resize(index + 1);
            }
            _values[index] = value;
        }

        void addSubValue(int index, const QString& value)
        {
            _subValues[index].append(value);
        }

        QStringList values() const { return _values; }
        QMap<int, QStringList> subValues() const { return _subValues; }

    private:
        QStringList _values;
        QMap<int, QStringList> _subValues;
    };

    QStringList _headers;
    Row _currentRow;
    QVector<Row> _rows;

    void createEmptyRow();
    int getColumnWidth(int index) const;
};

#endif // TEXTTABLE_H
