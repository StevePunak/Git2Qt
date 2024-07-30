#ifndef RENAMEDETAILS_H
#define RENAMEDETAILS_H

#include <QString>


namespace GIT {

class RenameDetails
{
public:
    RenameDetails() {}
    RenameDetails(const QString& oldFilePath, const QString& newFilePath, int similarity) :
        _oldFilePath(oldFilePath), _newFilePath(newFilePath), _similarity(similarity) {}

    QString oldFilePath() const { return _oldFilePath; }
    void setOldFilePath(const QString& value) { _oldFilePath = value; }

    QString newFilePath() const { return _newFilePath; }
    void setNewFilePath(const QString& value) { _newFilePath = value; }

    void setSimilarity(int value) { _similarity = value; }
    int similarity() const { return _similarity; }

private:
    QString _oldFilePath;
    QString _newFilePath;
    int _similarity = 0;
};

} // namespace GIT

#endif // RENAMEDETAILS_H
