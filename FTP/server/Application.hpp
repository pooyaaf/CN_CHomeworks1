#include "Configuration.hpp"

class Application
{
private:
    Configuration configuration;
public:
    void Serve(int commandSocket, int dataSocket);
};