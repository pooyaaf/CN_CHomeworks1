#include <unistd.h>
#include <fstream>
#include <iostream>
#include "Commands.hpp"
#include "../../utility.hpp"

void user(AppContext *context, string arg, Configuration configuration)
{
    vector<struct Auth> &auth = configuration.auth;
    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].name == arg)
        {
            context->user = new User(arg, auth[i].admin, auth[i].size);
            write_string(LOGIN_USER_SUCCESS, context->commandFd);
            loggin(make_log(USER, "", arg, true, LOGIN_USER_SUCCESS));
            return;
        }
    }
    write_string(LOGIN_INVALID_ERROR, context->commandFd);
    loggin(make_log(USER, "", arg, false, LOGIN_INVALID_ERROR));
}

void pass(AppContext *context, string arg, Configuration configuration)
{
    vector<struct Auth> &auth = configuration.auth;
    for (int i = 0; i < auth.size(); i++)
    {
        if (auth[i].pass == arg)
        {
            context->user->setRegistered();
            write_string(LOGIN_PASS_SUCCESS, context->commandFd);
            loggin(make_log(PASS, context->user->get_name(), arg, true, LOGIN_PASS_SUCCESS));
            return;
        }
    }
    write_string(LOGIN_INVALID_ERROR, context->commandFd);
    loggin(make_log(PASS, context->user->get_name(), arg, false, LOGIN_INVALID_ERROR));
}

void help(AppContext *context, string arg, Configuration configuration)
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
    write_string(help, context->commandFd);
    if (context->user != NULL && context->user->get_name() != "")
        loggin(make_log(HELP, context->user->get_name(), "", true, HELP_RETURN));
    else
        loggin(make_log(HELP, "", "", true, HELP_RETURN));
}

void quit(AppContext *context, string arg, Configuration configuration)
{
    write_string(QUIT_SUCCESS, context->commandFd);
    loggin(make_log(QUIT, context->user->get_name(), "", true, QUIT_SUCCESS));
    context->user = NULL;
}