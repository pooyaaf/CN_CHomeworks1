#pragma once
#include "User.hpp"
#include "Path.hpp"
#include <vector>
#include "../Configuration.hpp"

class AppContext
{
private:
public:
    User *user;
    Path path;
    int commandFd;
    int dataFd;
    vector<string> files;
    AppContext(Configuration configuration, int commandFd_, int dataFd_);
    void config_user(User *_user);
};