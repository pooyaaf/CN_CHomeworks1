#include <sys/socket.h>
#include "Application.hpp"
#include "context/AppContext.hpp"
#include "commands/HelpCommand.hpp"

void Application::Serve(int commandSocket, int dataSocket){
    AppContext context(configuration, commandSocket, dataSocket);

    HelpCommand commad;
    commad.Exectute(context);
}