#include "Command.hpp"
#include "../context/AppContext.hpp"
#include "unistd.h"
#include "../Configuration.hpp"

class UserCommand : Command
{
public:
    UserCommand(string _username, Configuration _config)
    {
        username = _username;
        auth = _config.auth;
    }
    void Exectute(AppContext context) override;

private:
    string username;
    vector<struct Auth> auth;
};

void UserCommand::Exectute(AppContext context)
{
    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].name == username)
        {
            string message = LOGIN_USER_SUCCESS;
            write(context.commandFd, message.c_str(), message.size());
            return;
        }
    }
    string message = LOGIN_INVALID_ERROR;
    write(context.commandFd, message.c_str(), message.size());
}