
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

#include "Messages.h"
#include "utility.hpp"
#include "Extra.h"

#define COMMAND_PORT 5000
#define DATA_PORT 6000

using namespace std;

// global variables
int data_fd, cmd_fd;

// move to extra
string get_cmd(string in)
{
    return in.substr(0, in.find(" "));
}

// replace with defines after mk
// move to extra
bool check_command(string in)
{
    size_t pos_start = 0, pos_end, delim_len = 1;
    string token;
    vector<string> res;

    while ((pos_end = in.find(" ", pos_start)) != string::npos)
    {
        token = in.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(in.substr(pos_start));

    if (res[0] == "help" || res[0] == "quit")
    {
        return res.size() == 1;
    }
    if (res[0] == "user" || res[0] == "pass" || res[0] == "retr")
    {
        return res.size() == 2;
    }
   
    return false;
}

// move to exta
// use defines in Message.h
bool is_data_command(string in)
{
    string cmd = get_cmd(in);
    return cmd == "retr";
}

int main(int argc, char const *argv[])
{
    if (argc > 2)
    {
        cmd_fd = connectTo(atoi(argv[1]));
        cout << "200: Cmd port connected: " << argv[1] << " fd : " << cmd_fd << endl
             << flush;
        data_fd = connectTo(atoi(argv[2]));
        cout << "200: Data port connected: " << argv[2] << " fd : " << data_fd << endl
             << flush;
    }
    else
    {
        cout << GENERAL_ERROR << endl
             << flush;
        exit(0);
    }

    if (cmd_fd < 0 || data_fd < 0)
    {
        cout << GENERAL_ERROR << endl
             << flush;

        close(cmd_fd);
        close(data_fd);

        exit(0);
    }

    string buff;
    cout << "Enter command, for information use \"help\"." << endl
         << flush;
    while (true)
    {
        getline(cin, buff);
        if (check_command(buff) == false)
        {
            cout << "Error" << endl
                 << flush; // SYNTAX_ERROR
            buff.clear();
            continue;
        }

        // send size of buff
        size_t buffLength = buff.size();
        send(cmd_fd, &buffLength, sizeof(size_t), MSG_CONFIRM);

        // send command
        send(cmd_fd, buff.c_str(), buff.size(), MSG_CONFIRM);

        // ? Waits ...

        // recive size of data
        string receivedString;
        read_string(&receivedString, cmd_fd);

        size_t dataLength;
        // if needs connect to data port
        if (is_data_command(buff) == true)
        {
            // recive size of input
            size_t bytesLength;
            recv(data_fd, &bytesLength, sizeof(size_t), 0);
            bytesLength = ntohl(bytesLength);

            // recive message
            vector<uint8_t> rcvBytes;
            rcvBytes.resize(bytesLength, 0x00);

            recv(data_fd, &(rcvBytes[0]), dataLength, 0);

            string convert;
            convert.assign(rcvBytes.begin(), rcvBytes.end());

           //retr
                cout << "DONE" << endl
                     << flush;
            
        }
        else
        {
            cout << receivedString << endl;
        }

        if (receivedString == "221: Successful Quit.") // QUIT_SUCCESS
        {
            close(cmd_fd);
            close(data_fd);
            break;
        }

        buff.clear();
    }
    cout << "System shut down..." << endl;
    return 0;
}