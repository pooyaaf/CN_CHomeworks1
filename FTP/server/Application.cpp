#include <iostream>
#include <unistd.h>
#include<unordered_map>

#include "Application.hpp"
#include "context/AppContext.hpp"
#include "commands/Commands.hpp"
#include "../utility.hpp"

#define BUF_SIZE 50

unordered_map<string, command> handlers = {

    {"pass", &pass},
    {"user", &user},
    {"help", &help},
    {"quit", &quit},
};

void Application::Serve(int command_socket, int data_socket)
{
    AppContext context(configuration, command_socket, data_socket);
    
    cout << "client registered" << endl;
    while (true)
    {
        string command;

        read_string(&command, command_socket);

        cout << "command " << command << endl;

        if(command.size() == 0)
        {
            close(command_socket);
            close(data_socket);
            break;
        }

        int command_index = command.find_first_of(' ');

        (*handlers[command.substr(0, command_index)])
        (&context, command.substr(command_index+1), configuration);
    }
}