#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#include "utility.hpp"

#define BUF_SIZE 50

void readLine(char *dst, int socket_fd)
{
    memset(dst, 0, strlen(dst));
    char buf[50];
    if (recvfrom(socket_fd, buf, sizeof buf, 0, NULL, NULL) > 0)
    {
        strcat(dst, buf);
    }
}

void die(const char* scope)
{
    perror(scope);
    exit(0);
}

int connectTo(int port)
{
    int socket_fd;
    struct sockaddr_in address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        die("Socket Creation Faild");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = port;

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof address) < 0)
    {
        die("Connection Faild");
    }

    return socket_fd;
}

int createBroadcastHub(int port, struct sockaddr_in *address, socklen_t size)
{
    int socket_fd;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == 0)
    {
        die("Socket Creation Faild");
    }

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    address->sin_port = port;
    int enable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof enable);
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof enable);

    if (bind(socket_fd, (struct sockaddr *)address, size) < 0)
    {
        die("bind Failed");
    }

    return socket_fd;
}

void read_string(string* buffer, int fd){
    size_t datalen;
    char buf[BUF_SIZE] = {0};

    read(fd, &datalen, sizeof(size_t));

    while (datalen > 0)
    {
        if(datalen < BUF_SIZE)
        {
            read(fd, buf, datalen);
            buf[datalen] = '\0';
            datalen = 0;
        } else {
            read(fd, buf, (sizeof buf) - 1);
            datalen -= BUF_SIZE - 1;
        }
        buffer->append(buf);
    }
}

void write_string(string& buffer, int fd){
    size_t len = buffer.size();
    write(fd, &len, sizeof(size_t));
    write(fd, buffer.c_str(), buffer.size());
}