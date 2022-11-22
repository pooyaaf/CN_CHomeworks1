#include <unistd.h>
#include <fstream>
#include "Commands.hpp"
#include "../../utility.hpp"

void user(AppContext* context, string arg, Configuration configuration)
{
    vector<struct Auth>& auth = configuration.auth;

    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].name == arg)
        {
            string message = LOGIN_USER_SUCCESS;
            write(context->commandFd, message.c_str(), message.size());
            return;
        }
    }

    string message = LOGIN_INVALID_ERROR;
    write(context->commandFd, message.c_str(), message.size());
}

void pass(AppContext* context, string arg, Configuration configuration)
{
    vector<struct Auth>& auth = configuration.auth;
    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].pass == arg)
        {
            string message = LOGIN_PASS_SUCCESS;
            write(context->commandFd, message.c_str(), message.size());
            return;
        }
    }

    string message = LOGIN_INVALID_ERROR;
    write(context->commandFd, message.c_str(), message.size());
}

void help(AppContext* context, string arg, Configuration configuration)
{
    fstream help_file;

    string help;
    string line;

    help_file.open("help.txt", ios::in);
    if (!help_file)
    {
        string message = "No such file";
        write_string(message, context->commandFd);
    }
    else
    {
        while (getline(help_file, line))
            help += line + "\n";
    }
    help_file.close();

    write(context->commandFd, help.c_str(), help.size());
}

void quit(AppContext* context, string arg, Configuration configuration)
{
    context->user = NULL;
    string message ="221: Successfil Quit";
    write_string(message, context->commandFd);
}