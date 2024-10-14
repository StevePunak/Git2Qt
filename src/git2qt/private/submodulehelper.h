#ifndef SUBMODULEHELPER_H
#define SUBMODULEHELPER_H

#include <QStringList>
#include <QString>
#include <QList>


namespace GIT {

class Submodule;
class Repository;

class SubmoduleHelper
{
public:
    static bool removeSubmodule(Repository* repo, const Submodule& submodule);
};

class GitModulesFile
{
public:
    GitModulesFile(const QString& path);

    bool removeSubmodule(const QString& name);
    bool write();

    bool isValid() const { return _valid; }

private:
    class SubmoduleChunk
    {
    public:
        SubmoduleChunk(const QStringList& lines);

        QString name() const { return _name; }
        QString path() const { return _path; }
        QString url() const { return _url; }
        QStringList lines() const { return _lines; }

    private:
        static QString grabAfterEquals(const QString& from);

        QString _name;
        QString _path;
        QString _url;
        QStringList _lines;
    };

    QString _path;
    QList<SubmoduleChunk> _chunks;
    bool _valid;
};

} // namespace GIT

#endif // SUBMODULEHELPER_H
