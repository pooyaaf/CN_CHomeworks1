#include "Command.hpp"
#include "../context/AppContext.hpp"
#include "unistd.h"

class PwdCommand : Command
{
public:
    void Exectute(AppContext context) override;
};

void PwdCommand::Exectute(AppContext context)
{
    string dir = context.path.GetVirtualPath();
    string path = "257: " + dir + "\n";
    write(context.commandFd, path.c_str(), path.size());
}