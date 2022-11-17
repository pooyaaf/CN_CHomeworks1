#include <sys/socket.h>
#include "Application.hpp"
#include "context/AppContext.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ UserCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/PwdCommand.hpp"

void Application::Serve(int commandSocket, int dataSocket)
{
    AppContext context(configuration, commandSocket, dataSocket);

    PwdCommand commad;
    commad.Exectute(context);
  
}