#include <sys/socket.h>
#include "Application.hpp"
#include "context/AppContext.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ UserCommand.hpp"

void Application::Serve(int commandSocket, int dataSocket)
{
    AppContext context(configuration, commandSocket, dataSocket);

    UserCommand commad = UserCommand("Perriex", configuration);
    commad.Exectute(context);

}