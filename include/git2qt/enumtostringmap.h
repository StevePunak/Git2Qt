/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class copied from KanoopCommonQt library to remove dependency.
 *
 * The original exists and is maintained at:
 *
 *   git@github.com:StevePunak/KanoopCommonQt.git
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef ENUMTOSTRINGMAP_H
#define ENUMTOSTRINGMAP_H

#include <QMap>



namespace GIT {

template <class T>
class EnumToStringMap : public QMap<T, QString>
{
public:
    QString getString(T type, const QString& defaultValue = QString()) const
    {
        auto it = this->find(type);
        return it != this->end()
                   ? it.value()
                   : defaultValue;
    }

    T getType(const QString& name, const T defaultValue = T()) const
    {
        bool ok;
        int enumValue = name.toInt(&ok);
        if(ok) {
            return (T)enumValue;
        }

        for(auto it = EnumToStringMap<T>::constBegin();it != EnumToStringMap<T>::constEnd();it++) {
            QString value = it.value();
            if(value == name.toLower()) {
                return it.key();
            }
        }
        return defaultValue;
    }

    QList<T> getTypes() const
    {
        QList<T> result = EnumToStringMap<T>::keys();
        return result;
    }

    bool containsString(const QString& name, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const
    {
        typename EnumToStringMap<T>::const_iterator it = std::find_if(EnumToStringMap<T>::constBegin(), EnumToStringMap<T>::constEnd(), [name, cs](const QString& a)
        {
            return a.compare(name, a, cs) == 0;
        });
        if(it != EnumToStringMap<T>::constEnd()) {
            return true;
        }
        return false;
    }

};

} // namespace GIT

#endif // ENUMTOSTRINGMAP_H
