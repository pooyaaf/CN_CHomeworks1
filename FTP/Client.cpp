#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <iostream>

#include "./server/Messages.h"
#include "utility.h"
#include "Extra.h"

#define COMMAND_PORT 5000
#define DATA_PORT 6000

using namespace std;

// global variables
int data_fd, cmd_fd;

int main(int argc, char const *argv[])
{
    if (argv[1] > 0 && argv[2] > 0)
    {
        cmd_fd = connectTo(atoi(argv[1]));
        cout << "200: Cmd port connected: " << argv[1] << " fd : " << cmd_fd << endl;
        data_fd = connectTo(atoi(argv[2]));
        cout << "200: Data port connected: " << argv[2] << " fd : " << data_fd << endl;
    }
    else
    {
        cout << GENERAL_ERROR << endl;
        exit(0);
    }

    if (cmd_fd < 0 || data_fd < 0)
    {
        cout << GENERAL_ERROR << endl;

        close(cmd_fd);
        close(data_fd);

        exit(0);
    }

    string buff;
    cout << "Enter command, for information use \"help\"." << endl;
    while (true)
    {
        string buff, delimiter = " ";
        cin >> buff;

        string token = buff.substr(0, buff.find(delimiter));
    }
}