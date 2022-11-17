#include "User.hpp"
#include "Path.hpp"
#include "../Configuration.hpp"

class AppContext
{
private:
public:
    User* user;
    Path path;
    int commandFd;
    int dataFd;
    AppContext(Configuration configuration, int commandFd_, int dataFd_);
};