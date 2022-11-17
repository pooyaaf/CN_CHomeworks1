#include "Command.hpp"
#include "../context/AppContext.hpp"
#include "unistd.h"
#include "../Configuration.hpp"

class PassCommand : Command
{
public:
    PassCommand(string _pass, Configuration _config)
    {
        pass = _pass;
        auth = _config.auth;
    }
    void Exectute(AppContext context) override;

private:
    string pass;
    vector<struct Auth> auth;
};

void PassCommand::Exectute(AppContext context)
{
    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].pass == pass)
        {
            string message = LOGIN_PASS_SUCCESS;
            write(context.commandFd, message.c_str(), message.size());
            return;
        }
    }
    string message = LOGIN_INVALID_ERROR;
    write(context.commandFd, message.c_str(), message.size());
}