#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "Application.hpp"
#include "context/AppContext.hpp"
#include "commands/Commands.hpp"
#include "../utility.hpp"

unordered_map<string, command> handlers = {
    {"user", &user},
    {"pass", &pass},
    {"help", &help},
    {"quit", &quit},
    {"retr", &retr},
};

string read_string(int fd)
{
    string result;
    char c;
    bool start_found = false;
    while (read(fd, &c, 1) > 0)
    {

        if (!start_found && c == '"')
        {
            start_found = true;
            continue;
        }
        if (!start_found)
        {
            continue;
        }
        if (start_found && c == '"')
        {
            break;
        }
        result += c;
    }
    return result;
}

int read_number(int fd)
{
    int result = 0;
    char c;
    bool start_found = false;
    while (read(fd, &c, 1) > 0)
    {
        if (!start_found && c <= '9' && c >= '0')
        {
            start_found = true;
        }
        if (!start_found)
        {
            continue;
        }
        if (start_found && !(c <= '9' && c >= '0'))
        {
            break;
        }
        result *= 10;
        result += c - '0';
    }
    return result;
}

char read_symbol(int fd)
{
    char c;
    do
    {
        read(fd, &c, 1);
    } while (c == ' ' || c == '\n');

    return c;
}

struct Auth read_user(int file)
{
    struct Auth auth;
    string key;
    char symbol;
    do
    {
        key = read_string(file);
        if (key == "user")
        {
            auth.name = read_string(file);
        }
        if (key == "password")
        {
            auth.pass = read_string(file);
        }
        if (key == "admin")
        {
            auth.admin = read_string(file) == "true";
        }
        if (key == "size")
        {
            auth.size = atoi(read_string(file).c_str());
        }
        symbol = read_symbol(file);
    } while (symbol != '}');

    return auth;
}

Application::Application()
{
    int file = open("root/config.json", O_RDONLY, 0666);
    char symbol;

    string key;
    do
    {
        key = read_string(file);
        if (key == "commandChannelPort")
        {
            configuration.command_channel_port = read_number(file);
        }

        if (key == "dataChannelPort")
        {
            configuration.data_channel_port = read_number(file);
        }

        if (key == "users")
        {
            do
            {
                configuration.auth.push_back(read_user(file));
                symbol = read_symbol(file);
            } while (symbol == ',');
        }
        if (key == "files")
        {
            do
            {
                configuration.admin_files.push_back(read_string(file));
                symbol = read_symbol(file);
            } while (symbol == ',');
        }
    } while (key != "");
}

void Application::Serve(int command_socket, int data_socket)
{
    AppContext context(configuration, command_socket, data_socket);

    loggin("client registered");
    while (true)
    {
        string command;

        read_string(&command, command_socket);

        if (command.size() == 0)
        {
            loggin("sockets closed");
            close(command_socket);
            close(data_socket);
            break;
        }

        int command_index = command.find_first_of(' ');

        string cmd = command.substr(0, command_index);

        if (context.user == NULL)
        {
            if (cmd == PASS)
            {
                write_string(LOGIN_PASS_ERROR, context.commandFd);
                loggin(LOGIN_PASS_ERROR);
                continue;
            }
            if (cmd != USER && cmd != HELP)
            {
                write_string(AUTH_ERROR, context.commandFd);
                loggin(AUTH_ERROR);
                continue;
            }
        }
        else if (context.user != NULL && context.user->isRegistered() == false)
        {
            if (cmd != PASS && cmd != HELP)
            {
                write_string(AUTH_ERROR, context.commandFd);
                loggin(AUTH_ERROR);
                continue;
            }
        }

        (*handlers[cmd])(&context, command.substr(command_index + 1), configuration);
    }
}