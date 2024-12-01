#include "submodulehelper.h"

#include <gitexception.h>
#include <repository.h>
#include <utility.h>

using namespace GIT;

bool SubmoduleHelper::removeSubmodule(Repository* repo, const Submodule& submodule)
{
    Q_UNUSED(submodule)
    bool result = false;
    try
    {
        QString path = Utility::combine(repo->localPath(), ".gitmodules");

        GitModulesFile gitmodules(path);
        if(gitmodules.isValid() == false) {
            throw GitException("Failed to read .gitmodules");
        }

        if(gitmodules.removeSubmodule(submodule.name()) == false) {
            throw GitException(QString("Failed to remove %1 from .gitmodules").arg(submodule.name()));
        }

        if(gitmodules.write() == false) {
            throw GitException("Failed to write .gitmodules");
        }
        result = true;
    }
    catch(const GitException& e)
    {
        Log::logText(LVL_WARNING, e.message());
        repo->setErrorText(e.message());
    }
    return result;
}

// -------------------------------- GitModulesFile --------------------------------

GitModulesFile::GitModulesFile(const QString& path) :
    _path(path)
{
    try
    {
        QStringList lines;
        if(Utility::readAllLines(path, lines) == false) {
            throw GitException("Failed to read .gitmodules");
        }

        QStringList moduleLines;
        for(int i = 0;i < lines.count();i++) {
            QString line = lines.at(i);
            if(line.startsWith("[submodule ")) {
                if(moduleLines.count() > 0) {
                    _chunks.append(SubmoduleChunk(moduleLines));
                    moduleLines.clear();
                }
            }
            moduleLines.append(line);
        }
        if(moduleLines.count() > 0) {
            _chunks.append(SubmoduleChunk(moduleLines));
        }

        _valid = true;
    }
    catch(const GitException& e)
    {
        Log::logText(LVL_WARNING, e.message());
    }
}

bool GitModulesFile::removeSubmodule(const QString& name)
{
    bool result = false;
    auto it = std::find_if(_chunks.constBegin(), _chunks.constEnd(), [name](const SubmoduleChunk& c) { return c.name() == name; } );
    if(it != _chunks.constEnd()) {
        _chunks.erase(it);
        result = true;
    }
    return result;
}

bool GitModulesFile::write()
{
    QStringList lines;
    for(const SubmoduleChunk& chunk : _chunks) {
        lines.append(chunk.lines());
    }
    return Utility::writeAllLines(_path, lines);
}

// -------------------------------- SubmoduleChunk --------------------------------

GitModulesFile::SubmoduleChunk::SubmoduleChunk(const QStringList& lines) :
    _lines(lines)
{
    for(const QString& line : lines) {
        if(line.startsWith("[submodule ")) {
            int i1 = line.indexOf('\"');
            int i2 = line.lastIndexOf('\"');
            if(i1 > 0 && i2 > i1) {
                _name = line.mid(i1 + 1, (i2 - i1) - 1);
            }
        }
        else if(line.trimmed().startsWith("path")) {
            _path = grabAfterEquals(line);
        }
        else if(line.trimmed().startsWith("url")) {
            _url = grabAfterEquals(line);
        }
    }
}

QString GitModulesFile::SubmoduleChunk::grabAfterEquals(const QString& from)
{
    QString result;
    int index = from.indexOf('=');
    if(index > 0) {
        result = from.mid(index + 1).trimmed();
    }
    return result;
}
