#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>

namespace GIT {

class Repository;
class Commands
{
public:
    static Repository* clone(const QString& remoteUrl, const QString& localPath);
};

} // namespace GIT

#endif // COMMANDS_H
