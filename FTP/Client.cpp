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
#include <vector>

#include "server/Messages.h"
#include "utility.hpp"

using namespace std;

// global variables
int data_fd, cmd_fd;

string get_cmd(string in)
{
    return in.substr(0, in.find(" "));
}

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

    if (res[0] == HELP || res[0] == QUIT)
    {
        return res.size() == 1;
    }
    if (res[0] == USER || res[0] == PASS || res[0] == RETR)
    {
        return res.size() == 2;
    }
  
    return false;
}

bool is_data_command(string in)
{
    string cmd = get_cmd(in);
    return cmd == RETR ;
}

int main(int argc, char const *argv[])
{
    if (argc > 2)
    {
        cmd_fd = connect_to(atoi(argv[1]));
        cout << "200: Cmd port connected: " << argv[1] << " fd : " << cmd_fd << endl
             << flush;
        data_fd = connect_to(atoi(argv[2]));
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
            cout << SYNTAX_ERROR << endl
                 << flush;
            buff.clear();
            continue;
        }

        size_t buffLength = buff.size();
        send(cmd_fd, &buffLength, sizeof(size_t), MSG_CONFIRM);

        send(cmd_fd, buff.c_str(), buff.size(), MSG_CONFIRM);

        string receivedString;
        read_string(&receivedString, cmd_fd);

        size_t dataLength;
        if (is_data_command(buff) == true && receivedString.substr(0, 3) == "226")
        {
        
            if(get_cmd(buff) == RETR)
            {
                off_t bytesLength;
                string filename;

                read_string(&filename, data_fd);
                filename = "user/" + filename;
                read(data_fd, &bytesLength, sizeof(off_t));

                char buf[BUF_SIZE];
                int file = open(filename.c_str(), O_WRONLY | O_CREAT, 0777);
                int readsize = 0;
                while (bytesLength > 0)
                {
                    readsize = read(data_fd, buf, sizeof buf);
                    write(file, buf, readsize);
                    bytesLength -= BUF_SIZE;
                }
                close(file);

                cout << receivedString << endl;
            }
        }
        else
        {
            cout << receivedString << endl;
        }

        if (receivedString == QUIT_SUCCESS)
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