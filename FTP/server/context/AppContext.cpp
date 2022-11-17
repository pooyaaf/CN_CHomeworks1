#include "AppContext.hpp"

AppContext::AppContext(Configuration configuration, int commandFd_, int dataFd_)
    : path(configuration.baseDirectory)
{
    commandFd = commandFd_;
    dataFd = dataFd_;
}

void AppContext::config_user(User *_user)
{
    user = _user;
}