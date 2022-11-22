#include "Configuration.hpp"

class Application
{
public:
    Configuration configuration;
    Application();
    void Serve(int commandSocket, int dataSocket);
};