#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "../utility.hpp"

#include "context/Path.hpp"
#include "Application.hpp"

int start_socket(int port)
{
    int fd;
    struct sockaddr_in address;

    printf("starting server on port %d...\n", port);
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        die("Socket Creation Faild");
    }

    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = port;

    if (bind(fd, (struct sockaddr *)&address, sizeof address) < 0)
    {
        die("Bind Faild");
    }

    listen(fd, 100);
    
    return fd;
}

int main(int argc, char const *argv[])
{
    int server_command_fd, server_data_fd, client_command_fd, client_data_fd;
    Application application;

    server_command_fd = start_socket(atoi(argv[1]));
    server_data_fd = start_socket(atoi(argv[2]));

    for (;;)
    {
        if ((client_command_fd = accept(server_command_fd, NULL, NULL)) < 0)
        {
            perror("Accept Faild");
        }
        else
        {
            printf("socket opened\n");
        }

        if ((client_data_fd = accept(server_data_fd, NULL, NULL)) < 0)
        {
            perror("Accept Faild");
        }
        else
        {
            printf("socket opened\n");
        }

        //application.Serve(client_command_fd, client_data_fd);
        thread thread([&]{application.Serve(client_command_fd, client_data_fd);});
        thread.detach();
    }
    return 0;
}
