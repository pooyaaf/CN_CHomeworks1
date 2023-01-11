#ifndef CLIENT_H
#define CLIENT_H "CLIENT_H"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <vector>


#define CMD_SIGN "> "

const int CMD_MAX_LEN = 128; // Maximum command length.
const int CMD_OUT_LEN = 8048; // Maximum command length received in telecommunications. 
const int DATA_OUT_LEN = 8048; // Maximum data length received in telecommunications.
const char* IP_ADDRESS = "127.0.0.1"; // The address 127.0. 0.1 is the standard address for IPv4 loopback traffic.
const std::string CLIENT_ERR = "Client could not be initiate.";
const std::string CONFIG_ADDRESS = "configuration/config.json"; // Config file path.
const std::string CMD_OUTPUT_COUT = "Command output: ";

class Client 
{
public:
    Client() = default;

    bool initiate(int serverPort);

private:
    bool invalidInitiating(int serverPort);

    char cmd[CMD_MAX_LEN];
    char outputCmd[DATA_OUT_LEN] = {0};

    int clientCmdFileDes; // Client command file descriptor.

    static constexpr int MAX_COMMAND_LENGTH = 128;

};

#endif