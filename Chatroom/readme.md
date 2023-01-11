# CN_CA1_P3
computer networking projects Fall 1401 - CA1_part3:    
___
This program is implemented in object oriented c++ programming. For each entity in the program, separate classes are considered as follows:    

## Section 0 - Basic explanations about the logic of the program 
- ### [Server class](#server-class)
- ### [Client class](#client-class)
    
### <a name="server-class"></a> Server class
This class is responsible for setting sockets. Data channel and command channel are created in this section. This class starts executing incoming client commands with the help of a vector of commands.    
```cpp
#ifndef SERVER_H_
#define SERVER_H_ "SERVER_H"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <utility>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <string>
#include <chrono>
#include <ctime> 
#include <map>

#include "../library/Constant.h"


typedef std::map<int, std::pair<std::pair<int, int>, int>> MP;

class Server 
{
public:
    static Server* get_instance();
    bool initiate();
    
private:
    Server();
    int setupServer(int port);
    void distinguishCommand(int id, char* cmd);
    std::string getClientIDs();
    std::string connectOrder(int id, std::string name);
    std::string listOrder();
    std::string infoOrder(int id);
    std::string sendOrder(int senderID, int receiverID, std::vector<std::string> mess);
    std::string receiveOrder(int id);
    void updateDB(int id, std::string name);

    std::map<int, int> allClients;
    std::vector<std::string> commands;
    std::vector<std::pair<std::string, std::string>> clientsDB;
    int lastClientIndex =0;
    int lastMessageIndex =0;
    int isConnected =0;
    MP allClientsFD; // first val is client ID and second val is client connected(if 1->connected, 0->not)
    std::string async="";
    int asycID = -1;
    bool isExit = false;

    static Server* instance;
};

#endif
```
The private field commands(`vector<string>`) is implemented in such a way that even though users who enter from different clients can implement their own commands independently. In each client, only one user can use the service live, and the other user of this client must either wait for the online user to quit or log in from another client(one user per each client.out). Likewise, a user can log in from several clients and use the service.
The `allClients` field stores each new client `socket` that starts working as a `key` in the map whose value is the `lastClientIndex` field, and can display the result of the commands on the named ports, because finds out which user wants the output of his/her command on which client.
As mentioned, the `lastClientIndex` field keeps the index of the last client command(as ID). In fact, the file decryptor that is created and used as a map key in allClients can find its client command according to this field.
The `allClients` is each client in order ID(which specify the connected socket ID(CMD)), so it gets fd and give id, we also have `allClientsFD` which uses(gets) id(cuz it is in order) and give that client cmd and async cmd FD(socket ids) and `isConnected` for when client doesn't enter `connect` command and doesn't login, so we close the socket.     
The `lastMessageIndex` is for server message count(due to examples not needed).    
The `asycID` and `async` are related to the send order, we should send message to receiver(if he/she is online) immediately on async channel.    
The `isExit` is for when `exit` command entered and we should close socket and feedback status.    
The `static Server* get_instance();` is the constructor of the server class which make singleton server so the data of the server never changes, and with every implements we get the same instance. Its input is the configuration class, using which the commandPort and dataPort fields are set, and the log file that stores important server data is identified or created.  
```cpp
Server* Server::instance = nullptr;

Server* Server::get_instance() {
    if (instance == nullptr)
        instance = new Server();
    return instance;
}

 //just to make sure no other class instantiates Server
Server::Server()
{
}
```
The `initiate` is the main logic of the program and the `setupServer` function is the first one is used .
In this function, due to the length of the code, the main logic of the program is placed, and the rest are only explained. At the beginning of the function, using the setUpServer function, the command and async channels are created.
```cpp
int Server::setupServer(int port)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, 4);

    return server_fd;
}

```
Then we go in an infinite loop. Two things may happen in this loop:    
One. Entering a new client:
  If a new client enters, we create two special channels for this client through the `acceptClient` function, one of which is connected to the command channel and the other to the async channel. Then we assign these two channels to the client in the form of a pair. (We will need this later when we want to send the information). Then we assign a Command class to this new client that has entered to manage its incoming commands. This logic allows multiple users to log in at the same time and all of them can give commands at the same time and each one can run their own program.
```cpp
int acceptClient(int serverFd) 
{
    int clientFd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    clientFd = accept(serverFd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return clientFd;
}
```
two. Receiving messages from the client side:    
  Now, if the number of bits received from the user is equal to zero, it means that the user has left, so this client and its information must be deleted.
If the number of bits received was greater than zero, it means that the command has been entered, so the command must be recognized using the `distinguishCommand` function. Then the output of the command and data is sent to the clients from the relevant channels using the send call system.
```cpp
void Server::distinguishCommand(int id, char* cmd)
{
    vector<string> wordsOfCmd = splitter(cmd);
    CMDtype order = getCMDType(wordsOfCmd);
    
    switch (order)
    {
        case CONNECT:
            commands[id] = connectOrder(id, wordsOfCmd[NAME_IDX]);
            break;
        case LIST:
            commands[id] = listOrder();
            break;
        case INFO:
            commands[id] = infoOrder(stoi(wordsOfCmd[ID_IDX]));
            break;
        case SEND:
            commands[id] = sendOrder(id, stoi(wordsOfCmd[ID_IDX]), wordsOfCmd);
            break;
        case RECEIVE:
            commands[id] = receiveOrder(id);
            break;
        case QUIT:
            allClientsFD[id].second = 0;
            isExit = true;
            commands[id] = USER_SUCCESSFUL_LOGOUT_A;
            break;
        case ARGPAR_ERR:
            commands[id] = PARA_SYNTAX_ERROR_A;
            break;
        case NOT_EXIST:
            commands[id] = TOTAL_ERRORS_A;
            break;
    }
}

```
The `distinguishCommand` function helps distinguish user input commands using switch case.
The commands are in the order of the inputs of the project, and the command first prepares the outputs according to which client it is being executed on.
`ARGPAR_ERR` is for this reason, if the input command is not supported, we will report in the output that the wrong command was entered, and if the command is not supported at all, `NOT_EXIST` will happen.    
```cpp
void Server::updateDB(int id, string name)
{
    int count = clientsDB.size();
    for (int i = 0; i < count; i++)
        if (clientsDB[i].first == name)
            clientsDB[id].second = clientsDB[i].second;
}

string Server::connectOrder(int id, string name)
{
    string res = EMPTY;
    res = CONNECT_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    updateDB(id, name);
    clientsDB[id].first = name;
    allClientsFD[id].second = 1;
    return res;
}
```
The `updateDB` function check if user name is repeated, get history of messages. then it prepare feedback and save name and make status of client to `isConnected =1`.    
```cpp
string Server::getClientIDs()
{
    string res = "";
    for (int i = 0; i < lastClientIndex ; i++)
        res += TABER + to_string(i) + PREV_DIR + clientsDB[i].first + NEWL;

    return res;
}

string Server::listOrder()
{
    string res = EMPTY;
    res = LIST_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    res += getClientIDs();
    res += NEWL;
    return res;
}
```
In `getClientIDs` we prepare users id and user name, using clientsdb in server. Then we prepare output.    
```cpp
string Server::infoOrder(int id)
{
    string res = EMPTY;
    res = INFO_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    cerr << "id: " << id << " name: " << clientsDB[id].first << endl;
    res += id >= lastClientIndex ? EMPTY : clientsDB[id].first;
    res += NEWL;
    return res;
}
```
Due to index, we get the user name. The `clientsDB` first pair is name and second is connection status.    
```cpp
string Server::sendOrder(int senderID, int receiverID, vector<string> mess)
{
    string message_ = EMPTY;
    for (int i = MESSAGE_IDX; i < mess.size(); i++)
        message_ += mess[i] + DELIMITER;

    string res = EMPTY;
    res = SEND_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    string message = to_string(senderID) + PREV_DIR + clientsDB[senderID].first + COLON + message_;
    async = CMD_OUT_SIGN + message;
    message += NEWL;
    clientsDB[receiverID].second += message;
    int status = allClientsFD[receiverID].second;
    res += STATUS_CMD_A;
    res += to_string(status);
    res += status ? STATUS_SUCCESS_CMD_A : STATUS_FAILURE_CMD_A ;
    res += NEWL;

    // async message
    if (status)
        asycID = receiverID;
    
    return res;
}
```
At first we have vector of strings, so we attach them to get the new message. Using `async` we send message in async channel to receiver if he/she is online otherwise we store message in history and wait till receive query.    
```cpp

string Server::receiveOrder(int id)
{
    string res = EMPTY;
    res = RECEIVE_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    res += clientsDB[id].second;
    return res;
}
```
Just show history.     
```cpp
bool Server::initiate()
{
    int serverFd, serverAsyncFd, newSocket, newAsyncSocket; 
    char buffer[1024] = {0};

    
    fd_set master_set, working_set;
    
    serverFd = setupServer(SERVEER_PORT);
    serverAsyncFd = setupServer(SERVEER_PORT+4);

    if (serverFd < 0 || serverAsyncFd < 0)
        return 0;

    cerr << "server fd: " << serverFd << " server async fd: " << serverAsyncFd << endl;

    FD_ZERO(&master_set);
    FD_SET(serverFd, &master_set);
    int max_sd = serverFd;
    int activity;

    cerr << "Server is running.." << endl;

    while (true) 
    {
        memcpy(&working_set, &master_set, sizeof(master_set));
        //working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) 
        {
            if (FD_ISSET(i, &working_set)) 
            {                
                if (i == serverFd) 
                {  // new client
                    
                    newSocket = acceptClient(serverFd);
                    newAsyncSocket = acceptClient(serverAsyncFd);

                    allClientsFD[lastClientIndex] = make_pair(make_pair(newSocket, newAsyncSocket), NOT_CONNECTED);
                    clientsDB.push_back(make_pair(EMPTY, EMPTY));
                    commands.push_back(EMPTY);
                    allClients[newSocket] = lastClientIndex++;

                    FD_SET(newSocket, &master_set);
                    if (newSocket > max_sd)
                        max_sd = newSocket;
                    
                    cerr << "New client connected. fd = " << newSocket << endl;
                }
                
                else 
                { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);

                    if ((bytes_received > 0)) 
                    {
                        distinguishCommand(allClients[i], buffer);
                        isConnected = allClientsFD[allClients[i]].second;
                        
                        cerr << "----------------------" << NEWL <<(commands[allClients[i]]) << "----------------------" << NEWL ;
                        //cmd
                        if (isConnected || isExit)
                            send(allClientsFD[allClients[i]].first.first, 
                                (commands[allClients[i]].c_str()), 
                                strlen(commands[allClients[i]].c_str()), 0);
                        
                        if (asycID>-1)
                            send(allClientsFD[asycID].first.second, 
                                (async.c_str()), 
                                strlen(async.c_str()), 0);
                        asycID = -1;
                        isExit = false;
                    }
                    if ((bytes_received == 0)||(isConnected == 0))
                    { // EOF
                        cerr << "Client closed with fd: " << i << ", isConnected: " << isConnected << endl;
                        allClientsFD[allClients[i]].second = 0;
                        string feedback = isConnected ? USER_SUCCESSFUL_LOGOUT_A : NO_USER_FOR_CMD_A;
                        send(allClientsFD[allClients[i]].first.first, 
                            (feedback.c_str()), 
                            strlen(feedback.c_str()), 0);
                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd)
                            while (FD_ISSET(max_sd, &master_set) == 0)
                                max_sd -= 1;
                        continue;
                    }

                    cerr << "Client " << i << ": " << buffer << endl;
                    memset(buffer, 0, 1024);
                }
            }
        }
    }
}
```
### <a name="client-class"></a> Client class
The private fields of this class are `clientDataFileDes` and `clientCmdFileDes`, which are ports for data and command channels. There are three char arrays to store command, command channel output and data channel output respectively.
```cpp#ifndef CLIENT_H
#define CLIENT_H "CLIENT_H"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <vector>


#define CMD_OUT_SIGN "<< "
#define CMD_IN_SIGN ">>"
#define BACK_DEL '\b'

const int CMD_MAX_LEN = 128; // Maximum command length.
const int CMD_OUT_LEN = 8048; // Maximum command length received in telecommunications. 
const int DATA_OUT_LEN = 8048; // Maximum data length received in telecommunications.
const char* IP_ADDRESS = "127.0.0.1"; // The address 127.0. 0.1 is the standard address for IPv4 loopback traffic.
const std::string CLIENT_ERR = "Client could not be initiate.";
const std::string CONFIG_ADDRESS = "configuration/config.json"; // Config file path.
const std::string CMD_OUTPUT_COUT = "Command output: ";

    char cmd[CMD_MAX_LEN];
    char outputCmd[DATA_OUT_LEN] = {0};
    char outputAsyncCmd[DATA_OUT_LEN] = {0};

    int clientCmdFileDes, clientCmdAsyncFileDes; // Client command file descriptor.

    static constexpr int MAX_COMMAND_LENGTH = 128;

class Client 
{
public:
    Client() = default;

    bool initiate(int serverPort);

private:
    bool invalidInitiating(int serverPort);

};

#endif
```
The `invalidInitiating` function checks whether a client can be formed or not, and if there are no problems, the new client is added to the channel.    
```cpp
bool Client::invalidInitiating(int commandPort)
{
    clientCmdFileDes = socket(AF_INET, SOCK_STREAM, 0);
    if (clientCmdFileDes < 0)
        return true;
    struct sockaddr_in serverCmdAddr;
    serverCmdAddr.sin_family = AF_INET;
    serverCmdAddr.sin_port = htons(commandPort);
    serverCmdAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    if (inet_pton(AF_INET, IP_ADDRESS, &serverCmdAddr.sin_addr) <= 0)
        return true;
    if (connect(clientCmdFileDes, (struct sockaddr*)&serverCmdAddr, sizeof(serverCmdAddr)) < 0)
        return true;
    
    clientCmdAsyncFileDes = socket(AF_INET, SOCK_STREAM, 0);
    if (clientCmdAsyncFileDes < 0)
        return true;
    struct sockaddr_in serverDataAddr;
    serverDataAddr.sin_family = AF_INET;
    serverDataAddr.sin_port = htons(commandPort+4);
    serverDataAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    if (inet_pton(AF_INET, IP_ADDRESS, &serverDataAddr.sin_addr) <= 0)
        return true;
    if (connect(clientCmdAsyncFileDes, (struct sockaddr*)&serverDataAddr, sizeof(serverDataAddr)) < 0)
        return true;
    
    return false;
}
```
In `initiate`, the client checks whether it can connect to the server using the invalidInitiating function. If it can, the client is created and then using thread it enters an endless loop that enters its commands and receives the output of the command channel and then the output of the async channel through two recvs.    
Cuz server gives service and other client in online status give message, so they are parallel.
```cpp
void* recieveTH(void* input)
{
    while (true) 
    {
        // Receive command from command line.

        cout << CMD_IN_SIGN;
        memset(cmd, 0, MAX_COMMAND_LENGTH);
        cin.getline(cmd, CMD_MAX_LEN);

        // Send command to server.
        send(clientCmdFileDes, cmd, CMD_MAX_LEN, 0);

        // Receive command output from server and show in command line.
        memset(outputCmd, 0, sizeof outputCmd);
        recv(clientCmdFileDes, outputCmd, sizeof(outputCmd), 0);
        cout << CMD_OUTPUT_COUT << outputCmd << endl;

        if (outputCmd == "Successful Quit.\n" )
            break;
        
        // Receive data output from server and show in command line.
    }
    pthread_exit(0);
}

void* recieveAsyncTH(void* input)
{
    while (true) 
    {
        memset(outputAsyncCmd, 0, sizeof outputAsyncCmd);
        recv(clientCmdAsyncFileDes, outputAsyncCmd, sizeof(outputAsyncCmd), 0);
        if (outputAsyncCmd[0] == '<')
        {
            cout << BACK_DEL << BACK_DEL << BACK_DEL << outputAsyncCmd << endl;
            cout << CMD_IN_SIGN;
        }
        if (outputCmd == "Successful Quit.\n" )
            break;
    }

    pthread_exit(0);
}

bool Client::initiate(int serverPort) 
{
    // Unsuccessful in initiating. 
    if (invalidInitiating(serverPort))
        return false;

    pthread_t recieverTH;
    pthread_t recieveATH;

    pthread_create( &recieverTH, NULL, recieveTH, NULL);
    pthread_create( &recieveATH, NULL, recieveAsyncTH, NULL);

    pthread_join(recieverTH, NULL);
    pthread_join(recieveATH, NULL);

    close(clientCmdFileDes);
    return true;
}
```
## Section 1- Output
    
 ![s1-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/6979ea9cbacf645eb5043af9f9920776790d12e9/Chatroom/IMG/s1-0.png "s1-0")     

 ![s1-1](https://github.com/pooyaaf/CN_CHomeworks1/blob/6979ea9cbacf645eb5043af9f9920776790d12e9/Chatroom/IMG/s1-1.png "s1-1") 

