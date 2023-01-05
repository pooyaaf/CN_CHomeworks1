# CN_CA1_P1
computer networking projects Fall 1401 - CA1_part1:    
GOAL: learning basic concepts of Socket programming,    
and how to design program and their relations so that we implement 3 phases with one Local Host and several Ports.    
FTP server: simple FTP server with 2 Client & Server parts, in order to control downloading and uploading files with a certain size(1024kbytes).
___
This program is implemented in object oriented c++ programming. For each entity in the program, separate classes are considered as follows:    

## Section 0 - Basic explanations about the logic of the program 
- ### [Configuration class](#configuration-class)
- ### [Server class](#server-class)
- ### [Client class](#client-class)
- ### [User class](#user-class)
- ### [Command class](#command-class)

### <a name="configuration-class"></a> Configuration class
This class is the class which our program starts with. It receives information from the `config.json` file in base folder and sets this information(using tree) in the private fields `users: customers who use this program`, `files`: only admin has access to these, `commandPort`: in config.json file a fixed value is set for it and it is one of the Server channels that sends data to the client, `dataPort`: in config.json file a fixed value is set for it and it is one of the Server channels that sends data to the client. `orgAddr` is a private field, which initializes program base address(using cwd command).    
The public functions of this program are private field getters and constructor.    
```cpp
#ifndef CONFIG_H_
#define CONFIG_H_ "CONFIG_H"

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>                                        
#include <boost/property_tree/json_parser.hpp>

#include "User.h"

class Configuration
{
public:
    Configuration(std::string path);
    int getCommandPort();
    int getDataPort();
    std::vector<User*> getUser();
    std::vector<std::string> getFile();
    std::string getOrgAddr();

private:
    std::vector<User*> users;
    std::vector<std::string> files;
    int commandPort;
    int dataPort;
    std::string orgAddr;
};

#endif
```    
Using library we read json and construct config class.    
```cpp
Configuration::Configuration(string path)
{
    ptree tree;                                                    
    read_json(path, tree);

    char addr[256];
    getcwd(addr, 256);
    orgAddr = addr;

    commandPort = tree.get_child("commandPort").get_value<int>();
    dataPort = tree.get_child("dataPort").get_value<int>();

    ptree userTree = tree.get_child("users");
    for (auto& item : userTree.get_child("")) 
    {
        string username = item.second.get<string>("user");
        string pass = item.second.get<string>("password");
        bool isAdmin = item.second.get<bool>("admin");
        int downladSize = item.second.get<int>("size");
        users.push_back(new User(username, pass, isAdmin, downladSize, orgAddr));
    }

    ptree fileTree = tree.get_child("files");
    for (auto& item : fileTree.get_child(""))
        files.push_back(item.second.get_value<string>());
}
```
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
#include "../library/Command.h"
#include "../library/Config.h"

#define FILE_RDONLY  0x000
#define FILE_WRONLY  0x001
#define FILE_RDWR    0x002
#define FILE_CREATE  0x200

const std::string NO_LOG_FILE = "Unable to open the log file, log file doen't exist. Creating new log file.\n";
const char* LOG_FILE_NAME = "log.txt";

typedef std::map<int, std::pair<int, int>> MP;

class Server 
{
public:
    static Server* get_instance();
    bool initiate();
    int setupServer(int port);
    void distinguishCommand(int socketFD, char* cmd);
    bool isLogin();
    void setDirectory(std::string newDir);
    
private:
    Server(Configuration configuration);
    void createLogFile();
    void addToLog(std::string action);

    Configuration config;
    std::vector<Command*> commands;
    std::map<int, int> allClients;
    int lastClientIndex =0;
    std::fstream logFile;
    MP allClientsFD;
    int commandPort;
    int dataPort;

    static Server* instance;
};

#endif
```
The private fields of this class include data port(`dataPort`) and command port(`commandPort`), which are used to create data and command channels.
The `logFile` field is a file where system activity information is stored(this section will be explained more in logging).
The vector field of commands(`vector<*Command>`) is implemented in such a way that even though users who enter from different clients can implement their own commands independently. In each client, only one user can use the service live, and the other user of this client must either wait for the online user to quit or log in from another client(one user per each client.out). Likewise, a user can log in from several clients and use the service.
The `allClients` field stores each new client `socket` that starts working as a `key` in the map whose value is the `lastClientIndex` field, and can display the result of the commands on the named ports, because finds out which user wants the output of his/her command on which client.
As mentioned, the `lastClientIndex` field keeps the index of the last client command(as ID). In fact, the file decryptor that is created and used as a map key in allClients can find its client command according to this field.
The `allClients` is each client in order ID(which specify the connected socket ID(CMD)), so it gets fd and give id, we also have `allClientsFD` which uses(gets) id(cuz it is in order) and give that client cmd and data FD(socket ids).     
The private function `createLogFile` starts working at the beginning of creating the server and creates or opens the said `log.txt` file, the point here is that because the server knows about its users, we programmed it in such a way that if the file is already created and had data, in the continuation of those, new activities append to the end.    
```cpp
void Server::createLogFile()
{
    logFile.open(LOG_FILE_NAME, fstream::in | fstream::out | fstream::app);

    if (!logFile) 
    {
        cout << NO_LOG_FILE;
        logFile.open(LOG_FILE_NAME,  fstream::in | fstream::out | fstream::trunc);
        logFile.close();
    } 
}
```
The private function `addToLog` saves the current activity coming from the command in the log file.    
Let explain the public fields of this class:    
```cpp
void Server::addToLog(string action)
{
    system_clock::time_point currentTime = system_clock::now();
    time_t cT = system_clock::to_time_t(currentTime);
    logFile.open(LOG_FILE_NAME, fstream::in | fstream::out | fstream::app);
     
    logFile << ctime(&cT);
    logFile << action;
    logFile.close();

}
```
The `static Server* get_instance();` is the constructor of the server class which make singleton server so the data of the server never changes, and with every implements we get the same instance. Its input is the configuration class, using which the commandPort and dataPort fields are set, and the log file that stores important server data is identified or created.  
```cpp
Server* Server::instance = nullptr;

Server* Server::get_instance() {
    Configuration config = Configuration(PATH);
    if (instance == nullptr)
        instance = new Server(config);
    return instance;
}
```
```cpp
 //just to make sure no other class instantiates Server
Server::Server(Configuration configuration) : config(configuration)
{
    
    commandPort = configuration.getCommandPort();
    dataPort = configuration.getDataPort();
    createLogFile();
}
```
The `initiate` is the main logic of the program and the `setupServer` function is the first one is used .
In this function, due to the length of the code, the main logic of the program is placed, and the rest are only explained. At the beginning of the function, using the setUpServer function, the data and command channels are created.
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
  If a new client enters, we create two special channels for this client through the `acceptClient` function, one of which is connected to the command channel and the other to the data channel. Then we assign these two channels to the client in the form of a pair. (We will need this later when we want to send the information). Then we assign a Command class to this new client that has entered to manage its incoming commands. This logic allows multiple users to log in at the same time and all of them can give commands at the same time and each one can run their own program.
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
void Server::distinguishCommand(int socketFD, char* cmd)
{
    vector<string> wordsOfCmd = splitter(cmd);
    commands[socketFD]->setWordOfCommand(wordsOfCmd);
    CMDtype order = getCMDType(wordsOfCmd);
    
    switch (order)
    {
        case USER:
            addToLog(commands[socketFD]->loginUser(allClientsFD[socketFD].first));
            break;
        case PASS:
            addToLog(commands[socketFD]->checkPass(allClientsFD[socketFD].first));
            break;
        case PWD:
            commands[socketFD]->getCurrentDirectory();
            break;
        case MKD:
            addToLog(commands[socketFD]->makeNewDirectory());
            break;
        case DELE:
            addToLog(commands[socketFD]->delDirectory());
            break;
        case LS:
            commands[socketFD]->getFileList();
            break;
        case CWD:
            commands[socketFD]->changeDirectory(wordsOfCmd.size());
            break;
        case RENAME:
            commands[socketFD]->renameFile();
            break;
        case RETR:
            addToLog(commands[socketFD]->downloadFile());
            break;
        case UPLOAD:
            addToLog(commands[socketFD]->uploadFile());
            break;
        case HELP:
            commands[socketFD]->help();
            break;
        case QUIT:
            commands[socketFD]->quit();
            break;
        case ARGPAR_ERR:
            commands[socketFD]->setInvalActivity(PARA_SYNTAX_ERROR_A);
            break;
        case NOT_EXIST:
            commands[socketFD]->setInvalActivity(TOTAL_ERRORS_A);
            break;
    }
}

```
The `distinguishCommand` function helps distinguish user input commands using switch case.
The commands are in the order of the inputs of the project, and the command first prepares the outputs according to which client it is being executed on.
`ARGPAR_ERR` is for this reason, if the input command is not supported, we will report in the output that the wrong command was entered, and if the command is not supported at all, `OT_EXIST` will happen.
```cpp
bool Server::initiate()
{
    int commandServerFd, dataServerFd, newCommandSocket, newDataSocket; 
    char buffer[1024] = {0};

    
    fd_set master_set, working_set;
    
    commandServerFd = setupServer(commandPort);
    dataServerFd = setupServer(dataPort);
    if (commandServerFd < 0 || dataServerFd < 0)
        return 0;

    // cerr << commandServerFd << " " << dataServerFd << endl;

    //fd_set read_fds, copy_fds;
    FD_ZERO(&master_set);
    FD_SET(commandServerFd, &master_set);
    int max_sd = commandServerFd;
    int activity;
    char received_buffer[128] = {0};

    write(1, "Server is running\n", 18);

    while (true) 
    {
        memcpy(&working_set, &master_set, sizeof(master_set));
        //working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) 
        {
            if (FD_ISSET(i, &working_set)) 
            {                
                if (i == commandServerFd) 
                {  // new client
                    
                    newCommandSocket = acceptClient(commandServerFd);
                    newDataSocket = acceptClient(dataServerFd);
                    // if(newCommandSocket == 3 | newDataSocket == 3)
                        // newCommandSocket = ++max_sd , newDataSocket = ++max_sd;
                    allClientsFD[lastClientIndex] = make_pair(newCommandSocket, newDataSocket);
                    allClients[newCommandSocket] = lastClientIndex++;
                    commands.push_back(new Command(config.getUser(), config.getFile(), config.getOrgAddr()));              
                    FD_SET(newCommandSocket, &master_set);
                    if (newCommandSocket > max_sd)
                        max_sd = newCommandSocket;
                    // printf("New client connected. fd = %d\n", newCommandSocket);
                }
                
                else 
                { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    
                    if (bytes_received == 0) 
                    { // EOF
                        // printf("client fd = %d closed\n", i);
                        commands[allClients[i]]->setUserQuit();

                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd)
                            while (FD_ISSET(max_sd, &master_set) == 0)
                                max_sd -= 1;
                        continue;
                    }
                    else if (bytes_received > 0) 
                    {
                        distinguishCommand(allClients[i], buffer);
                        
                        //cmd
                        send(allClientsFD[allClients[i]].first, 
                            (commands[allClients[i]]->getCmdChannel().c_str()), 
                            strlen(commands[allClients[i]]->getCmdChannel().c_str()), 0);
                        //data
                        // cerr << "----------------------" << NEWL <<(commands[allClients[i]]->getDataChannel()) << "----------------------" << NEWL ;
                        // cerr << "++++++++++++++++++++++" << NEWL <<(commands[allClients[i]]->getDataChannel().c_str()) << "++++++++++++++++++++" << NEWL ;
                        send(allClientsFD[allClients[i]].second, 
                            (commands[allClients[i]]->getDataChannel().c_str()), 
                            strlen(commands[allClients[i]]->getDataChannel().c_str()), 0);
                    }

                    memset(buffer, 0, 1024);
                }
            }
        }
    }
}
```
### <a name="client-class"></a> Client class
The private fields of this class are `clientDataFileDes` and `clientCmdFileDes`, which are ports for data and command channels. There are three char arrays to store command, command channel output and data channel output respectively.
```cpp
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

#include "Config.h"

#define PATH  "config.json"
#define CMD_SIGN "> "

const int CMD_MAX_LEN = 128; // Maximum command length.
const int CMD_OUT_LEN = 8048; // Maximum command length received in telecommunications. 
const int DATA_OUT_LEN = 8048; // Maximum data length received in telecommunications.
const char* IP_ADDRESS = "127.0.0.1"; // The address 127.0. 0.1 is the standard address for IPv4 loopback traffic.
const std::string CLIENT_ERR = "Client could not be initiate.";
const std::string CONFIG_ADDRESS = "configuration/config.json"; // Config file path.
const std::string CMD_OUTPUT_COUT = "Command output: ";
const std::string DATA_OUTPUT_COUT = "Data output: ";

class Client 
{
public:
    Client() = default;

    bool initiate(int commandPort, int dataPort);

private:
    bool invalidInitiating(int commandPort, int dataPort);
    bool ISserverActive();

    char cmd[CMD_MAX_LEN];
    char outputCmd[DATA_OUT_LEN] = {0};
    char outputData[CMD_OUT_LEN] = {0};

    int clientCmdFileDes; // Client command file descriptor.
    int clientDataFileDes; // Client command file descriptor.

    static constexpr int MAX_COMMAND_LENGTH = 128;

};

#endif
```
The `invalidInitiating` function checks whether a client can be formed or not, and if there are no problems, the new client is added to the channel.
```cpp
bool Client::invalidInitiating(int commandPort, int dataPort)
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


    clientDataFileDes = socket(AF_INET, SOCK_STREAM, 0);
    if (clientDataFileDes < 0)
        return true;
    struct sockaddr_in serverDataAddr;
    serverDataAddr.sin_family = AF_INET;
    serverDataAddr.sin_port = htons(dataPort);
    serverDataAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    if (inet_pton(AF_INET, IP_ADDRESS, &serverDataAddr.sin_addr) <= 0)
        return true;
    if (connect(clientDataFileDes, (struct sockaddr*)&serverDataAddr, sizeof(serverDataAddr)) < 0)
        return true;
    
    return false;
}
```
In `initiate`, the client checks whether it can connect to the server using the invalidInitiating function. If it can, the client is created and then it enters an endless loop that enters its commands and receives the output of the command channel and then the output of the data channel through two recvs.
```cpp
bool Client::initiate(int commandPort, int dataPort) 
{
    // Unsuccessful in initiating. 
    if (invalidInitiating(commandPort,dataPort))
        return false;

    while (true) 
    {
        // Receive command from command line.
        cout << CMD_SIGN;
        memset(cmd, 0, MAX_COMMAND_LENGTH);
        cin.getline(cmd, CMD_MAX_LEN);

        // Send command to server.
        send(clientCmdFileDes, cmd, CMD_MAX_LEN, 0);

        // Receive command output from server and show in command line.
        memset(outputCmd, 0, sizeof outputCmd);
        recv(clientCmdFileDes, outputCmd, sizeof(outputCmd), 0);
        cout << CMD_OUTPUT_COUT << outputCmd << endl;

        // Receive data output from server and show in command line.
        memset(outputData, 0, sizeof outputData);
        recv(clientDataFileDes, outputData, sizeof(outputData), 0);
        cout << DATA_OUTPUT_COUT << outputData << endl;

    }

    return true;
}
```
### <a name="user-class"></a> User class
Explanation of private fields:
```cpp
#ifndef USER_H_
#define USER_H_ "USER_H"

#include <string>

#define NOT_ENTERED 0
#define WAIT_FOR_PASS 1
#define ENTERED 2

class User
{
public:
    User(std::string username_, std::string pass_, bool isAdmin_, 
            int downladSize_, std::string orgAddress_);
    std::string getName();
    std::string getPass();
    bool getIsAdmin();
    int getDSize();
    int getStatus();
    void setUserStatus(int status);
    void decDownloadSize(int decSize);
    bool isAbleToDownload(int decSize);
    std::string getOrgAddr();
    void setClientID(int clientID_);
    int getClientID();

private:
    std::string orgAddress;
    std::string username;
    std::string pass;
    bool isAdmin;
    int downladSize;
    int userStatus;
    int clientID;
};


#endif
```
`username`: User name, `pass`: Password, User address: `orgAddress`(project base), Is this user admin or not: `isAdmin`, The amount of size that this user can download: `downloadSize`, What is the status of this user? Is it not logged in? Either it is waiting for the login password or it is entered into the system: `userStatus`, the `clientID` field is because if the user has entered his username on the other side, he will enter the password from the same client. If the password is entered from the other client, it will be controlled.
Except for the last one, all other fields are extracted from the config.json file using configuration.    
Except getters of private fields, We will explain others.    
`setUserStatus`: This function updates the user status. (In fact, this function is a setter)    
`decDownloadSize`: This function reduces the user's download size.    
`isAbleToDownload`: This function returns true if the user has the right size to download the file, and returns false if there is no right size to download.
### <a name="command-class"></a> Command class
The functions related to project commands are explained in this section, how the commands are handled and get the answer, and then the answer for the data channel and the response and the log file are prepared and sent or received.    
```cpp
#ifndef COMMAND_H_
#define COMMAND_H_  "COMMAND_H"

#include "../library/User.h"
#include "../library/Constant.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

class Command
{
public:
    Command(std::vector<User*> users_, std::vector<std::string> files_, std::string addr_);
    std::string loginUser(int socket);
    std::string checkPass(int socket);
    void getCurrentDirectory();
    std::string makeNewDirectory();
    std::string delDirectory();
    void getFileList();
    void changeDirectory(int isOrg);
    void renameFile();
    std::string downloadFile();
    std::string uploadFile();
    void help();
    void quit();

    void setWordOfCommand(std::vector<std::string> wordsOfCmd_);
    void setDirectory(std::string newDir);
    bool isLogin();
    bool fileIsSecure(std::string fileName);
    void setUserQuit();
    void setInvalActivity(std::string activity);
    std::string getDataChannel();
    std::string getCmdChannel();

private:
    void resetChannels();

    std::vector<std::string> wordsOfCmd;
    std::vector<std::string> files;
    std::string currentDirectory;
    std::vector<User*> users;
    std::string dataChannel;
    std::string cmdChannel;
    int onlineUser;
};

#endif
```
The `isLogin` function is used to determine whether the user is logged in or not.
It can be identified using the `onlineUser` variable. In fact, this value contains the number of the user who is online, and if the user is not logged in, the value is negative one.    
```cpp
bool Command::isLogin()
{
    if(onlineUser == -1)
    {
        cmdChannel = NO_USER_FOR_CMD_A;
        return false;
    }
    else
        return true;
}

```
`setWordOfCommand` function: to set the private wordsOfCmd value. It is a normal setter function and I will skip its code.    
`setDirectory` function: to set the current directory of the program. is a normal setter function.    
The `fileIsSecure` function checks whether the file is among the files that only the administrator should have access to, or whether others can also access it.    
```cpp
bool Command::fileIsSecure(string fileName)
{
    for(int i = 0 ; i < files.size() ; i++)
    {
        if(files[i] == fileName)
            return true;
    } 
    return false;
}
```
It works in such a way that it checks all the files and if one of the files has the same name as this file, it is declared as a security file, so it returns the correct value. But on the other hand, if this file name is not the same as any of the files, the false value will be returned.    
`setUserQuit` function: In this function, the user's status is changed to not logged in and onlineUser (which indicates who is currently online and present in this client) is equal to -1.    
```cpp
void Command::setUserQuit()
{
    for(int i = 0 ; i < users.size() ; i++)
        users[i]->setUserStatus(NOT_ENTERED);
    onlineUser = -1;
}
```
The `setInvalActivity` function is to find out if the input command or command is not according to the standard set (arguments and parameters) or if the command was not entered at all and the data was outlier, and prepare the corresponding output.
```cpp
void Command::setInvalActivity(string activity)
{
    resetChannels();
    cmdChannel = activity;
    dataChannel = EMPTY;
}
```
The two channel getter functions that we used are for transferring appropriate outputs from the command to the client. The function `getDataChannel` is for the data channel and the next function `getCmdChannel` is for the response channel.
The rest of the functions also perform operations related to input commands, which are explained in the following sections.
The `resetChannels` function is to reset the channels in the command section of the string to the initial state, and then initialize the commands with their response and send them to the client.
```cpp
void Command::resetChannels()
{
    cmdChannel.clear();
    dataChannel.clear();

    cmdChannel = EMPTY;
    dataChannel = EMPTY;
}
```
## Section 1- Introduction
```json
{
    "commandPort": 8080,
    "dataPort":8081,
    "users": [
      {
          "user": "Reyhane",
          "password": "123",
          "admin": "true",
          "size": "1000000"
      },
      {
          "user": "Narges",
          "password": "321",
          "admin": "false",
          "size": "1000"
      }
    ],
      "files": [
        "config.json"
      ]
  }
```
```cpp
int main(int argc, char * argv[])
{
    if(!Server::get_instance()->initiate())
        cout << "The server could not be built" << endl;

    return 0;
}

```
We choose 2 ports for command and date server respectively.
In `Server.cpp` we run main(server), recording to instance we create, server become active and then clients can connect to the server. Using make file we have 2 client and server out part, so we run them in seperate terminal windows.    
 ![s1-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/beb955db9eabf447301a8235aabfdc6e140d32ab/RFTP/IMG/s1-0.png "s1-0") 
 ![s1-1](https://github.com/pooyaaf/CN_CHomeworks1/blob/beb955db9eabf447301a8235aabfdc6e140d32ab/RFTP/IMG/s1-1.png "s1-1") 
## Section 2- Authentication and access management
In `config.json`, we create system users information.
Since the authentication starts on the server, we create the configuration class on the server and use it to have the user information and store it as a vector from the user class.    
In the server, the task is divided and the `command.cpp` would done rest of the work, which performs the task/activity given to it by the server.    
Now we will examine the user and pass commands.    
user command:    
If the user enters the user command, it is first checked that the user has not been logged in before, because if he/her has logged in, the system will not accept the login again (because the commands do not have an ID, and there is no IP or something like that to distinguish the commands). After this, it checks whether the username exists in the system or not. which is printed if there is no corresponding error. If there is a user, a login message will be sent, and the user's status will change to waiting for a password. Also, we save the ID of the client from which the name of the user is entered to make sure that they are logged in to the same client.
In this section, we have a string called `action`, which according to the project, prepares information such as the person who entered for the `log file`.
```cpp
string Command::loginUser(int socket)
{
    resetChannels();
    string action = "";
    if(!isLogin())
    {
        bool check = false;
        for(int i = 0 ; i < users.size() ; i++)
            if(!wordsOfCmd[PARA1].compare(users[i]->getName()) && !users[i]->getStatus())
            {
                cmdChannel = USER_ACCEPTED_A;
                users[i]->setClientID(socket);
                users[i]->setUserStatus(WAIT_FOR_PASS);
                action += (users[i]->getName() + COLON + "Has entered username." + NEWL);
                check = true;
            }
        if(!check)
        {
            cmdChannel = INVALID_USER_PASS_A;
            action += ("User entered wrong username.\n");
        }
    }
    else
    {
        cmdChannel = TOTAL_ERRORS_A;
        action += ("User disturbing another user.\n");
    }
    return action;
}
```
pass command:
The user's password string is entered along with the pass command. First, we check that the password and username are in the same client. If there is a user whose status is waiting for a password, we check the password registered in the system for that user with the value of the password entered next to the pass command. If they were the same, we change the user's status to logged in and send a successful login message and change the onlineUser value to this user's number. Otherwise, if the clients are not the same and the password is wrong, we will give the password error message. It is also possible that no user has logged in, in which case the message "No user has logged in" will be sent.
Similarly, since we are in the login stage, we store this information in the `action` variable to be saved in the `log file`.
```cpp
string Command::checkPass(int socket)
{
    resetChannels();
    string action = "";
    if(!isLogin())
    {
        bool check = false;
        for(int i = 0 ; i < users.size() ; i++)
            if (socket == users[i]->getClientID())
                if(users[i]->getStatus() == WAIT_FOR_PASS)
                {
                    check = 1;
                    if(!wordsOfCmd[PARA1].compare(users[i]->getPass()))
                    {
                        cmdChannel = USER_SUCCESSFUL_LOGIN_A;
                        users[i]->setUserStatus(ENTERED);
                        onlineUser = i;
                        action += (users[i]->getName() + COLON + "Has successfully logged in." + NEWL);
                        break;
                    }
                    else
                    {
                        cmdChannel = INVALID_USER_PASS_A;
                        action += (users[i]->getName() + COLON + "Has entered wrong password." + NEWL);
                    }
                }
            
        if(!check)
        {
            cmdChannel = CLIENT_NOT_LOG_A;
            action += ("User hasn't enter username.\n");
        }
    }
    else
    {
        cmdChannel = TOTAL_ERRORS_A;
        action += ("User disturbing another user.\n");
    }
    return action;
}
```
See code outputs:
 ![s2-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/a30f6022b13f64dd483b14bd8633369dcdbf22ef/RFTP/IMG/s2-0.png "s2-0") 
At first, we enter the user in the above terminal and give a name outside of the config names and get the said error. Then, in the same terminal, we do not give the user argument and get the corresponding error. Now we enter the correct user name in the lower terminal, we enter the password of the same user to see that it is not entered from a different client, according to the output, this result was also achieved and the error that one must enter the name in order and then enter the password has been transferred. Then we ask for one of the services in the lower terminal, and since no one has logged in in the lower client, the error of entering the account is output. Then in the lower terminal we enter a command that is not supported at all (this is for later parts) and we get the corresponding error as expected. Then we enter the user's password in the upper terminal and get the output as expected, and in the lower terminal we enter the user's password again to correctly show the error of non-observance of order.    
 ![s2-1](https://github.com/pooyaaf/CN_CHomeworks1/blob/a30f6022b13f64dd483b14bd8633369dcdbf22ef/RFTP/IMG/s2-1.png "s2-1") 
In this section, the difference between user and admin is mentioned, and it is true that the command used will be introduced in the next sections, but we will use that command to show the output of this section. As we saw in the config.json file, the file that only has access to it is this file and the admin is called Reyhane, and other users should not have access to this file, and logically we do not expect that Narges user has access to it.
The code has already been explained that we have a function called isAdmin from which we can determine whether the user is able to perform the command or not.
## Section 3- Download file
‍‍‍‍‍‍    
```cpp
string Command::downloadFile()
{
    resetChannels();
    string action = "";
    if(isLogin())
    {
        if (fileIsSecure(wordsOfCmd[PARA1]) && !users[onlineUser]->getIsAdmin())
        {
            cmdChannel = FILE_ACCESS;
            action += (users[onlineUser]->getName() + COLON + 
                "Permission denied on file named " + wordsOfCmd[PARA1] + "." + NEWL);
            return action;
        }

        string directory = currentDirectory + wordsOfCmd[PARA1];
        ifstream readFile(directory, ios::binary);
        readFile.seekg(0, ios::end);
        int fileSize = readFile.tellg();
                
        if (!users[onlineUser]->isAbleToDownload(fileSize)||fileSize>UPLOAD_LIMIT)
        {
            cmdChannel = DOWNLOAD_LIMIT_A;
            string ISupload = fileSize>UPLOAD_LIMIT ? "file size limit" : "user connection";
            action += (users[onlineUser]->getName() + COLON + "Download failed due to " + ISupload + "." + NEWL);
            return action;
        }

        string whole = toStringConverter(directory);
        makeFileInDirectory(whole, wordsOfCmd[PARA1], "");

        cmdChannel = SUCCESSFUL_DOWNLOAD_A;
        dataChannel = whole;
        action += (users[onlineUser]->getName() + COLON + 
                    "Has successfully downloaded file named " + wordsOfCmd[PARA1] + "." + NEWL);
        users[onlineUser]->decDownloadSize(fileSize);
                
    }
    return action;
}
```
First, in the `resetchannels` function, we empty the output channels to save the result of the activity, then in `isLogin`, we check whether there is a user for the input command or not, and according to the result, it shows the errors we checked earlier. Now the permission of the file is checked for the user`fileIsSecure(wordsOfCmd[PARA1]) && !users[onlineUser]->getIsAdmin()`, if the file is confidential, the user must be an admin, otherwise it will get an error and the work will be finished. Then we get the size of the file and give it as input to the `isAbleToDownload` function and also check that it does not exceed the size mentioned in the project (1024 KB). If the value of true is returned from the function, it means that we have enough volume to download the file, otherwise, if each option is rejected, it is checked which option caused the failure and ends as a failed activity.
```cpp
void makeFileInDirectory(string context, string sourceDir, string destDir)
{
    fstream recent;
    ofstream res;
    string name = extractFileName(sourceDir);
    recent.open(destDir + name);
    // cerr<<"\nname, source, dest: "<< name << ", "<< sourceDir << ", " <<destDir<<"\n";

    if (!recent) // not exist
        res.open(destDir + name);
    else // existing file (wasnt necessary to handle duplicate)
        res.open(destDir + name);

    res << context;
    res.close();
}
```
If it is not finished, then we read the contents of the file and with the help of the `makeFileInDirectory` function, we download the file in the place where the user is and send the content to the user through the data channel and the command channel, which receives the success.
In the description of the `makeFileInDirectory` function, I should say that it has three inputs, the names of the `content`, the `source` path, and the `destination` path.
The content is obviously the same content that was read from the file. The source path is the input of the command, which may be any location that the user has given to download the file from, so its name must also be extracted if it is from another location that we have the name of the file when downloading, that is why we also have an `extractFileName` function. 
```cpp
string extractFileName(string addr)
{
    size_t pos = addr.find_last_of(BACK_SLASH);
    if (pos == string::npos)
        return addr;

    string res = addr.substr(pos+1, addr.size());
    return res;
}
```
Which checks according to the input address, if there is a slash, the name of the file is from the last slash, otherwise, if it does not have a slash, it is in the same directory and the file name is the same. The destination path is the place where the file should be saved, which is the same location as the user is in downloading, so we leave it empty, but in uploading, we have to give the address of the server files.    
 ![s3-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/ba1ef063257d00b2904f4315674a1dc0ec839499/RFTP/IMG/s3-0.png "s3-0") 
To show the result of the code, we use this method that the user Narges has a small volume and cannot download the c file, but the user Reyhane has enough volume to download the file, and we can also download the locked file by this user and see the result. On the other hand, the download of the file that is in the same place will not be seen because it has the same name and nothing has changed (of course, we duplicated it, but since it was not included in the project, we deleted it because it had different algorithms that were not needed). For this reason, we download another file from the external folder to complete it.
## Section 4- Upload file
    
```cpp
string Command::uploadFile()
{
    resetChannels();
    string action = "";
    if(isLogin())
    {
        if (fileIsSecure(wordsOfCmd[PARA1]) && !users[onlineUser]->getIsAdmin())
        {
            cmdChannel = FILE_ACCESS;
            action += (users[onlineUser]->getName() + COLON + 
                "Permission denied on file named " + wordsOfCmd[PARA1] + "." + NEWL);
            return action;
        }

        string directory = currentDirectory + wordsOfCmd[PARA1];
        ifstream readFile(directory, ios::binary);
        readFile.seekg(0, ios::end);
        int fileSize = readFile.tellg();
                
        if (!users[onlineUser]->isAbleToDownload(fileSize)||fileSize>UPLOAD_LIMIT)
        {
            cmdChannel = DOWNLOAD_LIMIT_A;
            string ISupload = fileSize>UPLOAD_LIMIT ? "file size limit" : "user connection";
            action += (users[onlineUser]->getName() + COLON + "Upload failed due to " + ISupload + "." + NEWL);
            return action;
        }


        string whole = toStringConverter(directory);
        string server = getServerDirectory(currentDirectory);

        makeFileInDirectory(whole, wordsOfCmd[PARA1], server);

        cmdChannel = SUCCESSFUL_DOWNLOAD_A;
        dataChannel = whole;
        action += (users[onlineUser]->getName() + COLON + 
                    "Has successfully uploaded file named " + wordsOfCmd[PARA1] + "." + NEWL);
        users[onlineUser]->decDownloadSize(fileSize);
                
    }
    return action;
}
```
This section is same as download section with one different, that the uploading location is in base directory, which we get it from `getServerDirectory` function which give the correct destination directory.    
 ![s4-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/3274ca6d2dd1c07711324a58e4aafa6a1564bdcd/RFTP/IMG/s4-0.png "s4-0")
For output we just show to uploading format, and server downloading dir named `Files`.
## Section 5- Help
We have a string which stores all instructions explenations.
```cpp
void Command::help()
{
    resetChannels();
    if(isLogin())
    {
        cmdChannel = HELP_TAG_A; 
        cmdChannel += USER_HELP_DEC;
        cmdChannel += PASS_HELP_DEC;
        cmdChannel += PWD_HELP_DEC;
        cmdChannel += MKD_HELP_DEC; 
        cmdChannel += DELE_HELP_DEC; 
        cmdChannel += LS_HELP_DEC; 
        cmdChannel += CWD_HELP_DEC; 
        cmdChannel += RENAME_HELP_DEC; 
        cmdChannel += RETR_HELP_DEC;
        cmdChannel += UPLOAD_HELP_DEC;
        cmdChannel += HELP_HELP_DEC; 
        cmdChannel += QUIT_HELP_DEC; 
    }
}
```
 ![s5-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s5-0.png "s5-0")
## Section 6- Quit
In this section, we change all user statuses to not logged in and also change the onlineUser number to -1, which means that no user is online anymore in this client.
```cpp
void Command::quit()
{
    resetChannels();
    if(isLogin())
    {
        users[onlineUser]->setUserStatus(NOT_ENTERED);
        onlineUser = -1;
        cmdChannel = USER_SUCCESSFUL_LOGOUT_A;
    }
}
```
 ![s6-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s6-0.png "s6-0")
## Section 7- Management of errors
In each section, if there is a possibility of an error, a check was made and an error message was sent if necessary.
But in summary, we show some of them:
 ![s7-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s7-0.png "s7-0")
First, in the lower terminal where there is no user, we enter the service command and get the expected output.
In the lower terminal, we enter the command command again, but in such a way that it is not defined and the type of definition and argument is wrong, and we output the corresponding error.
In the above terminal, we log in the user and in his login, we log in another user, and because the user is not logged out, we have an error.
Likewise, we define a completely irrelevant command and get an error.
## Section 8- Managing the volume of users
As explained in the download section, the `isAbleToDownload` function was used to check the sufficient volume for downloading, and after downloading, the corresponding file volume was deducted from the user's total volume.
```cpp
bool User::isAbleToDownload(int decSize)
{
    if(downladSize - decSize > 0)
        return true;
    return false;
}

```
The result of this section is exactly stated in the previous examples, so for convenience, we bring the same ones:
 ![s8-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s8-0.png "s8-0")
## Section 9- Client
Was explained in Client class.
## Section 10- Login
We designed this part so that if the file doesn't exist, it will create it and if it does exist, it will continue to write to it because we want the server memory to be saved, and we can check it if we need it in the future.    
With the help of time, we can find the current time. We did this part on the server because the activities are done with the help of the server and on the server, and when we put this on the server, we saw that it is like a backup and keeping the information of the server yard, that's why we kept it there.    
The initial creation of the file, which is done after the creation of the server, is how the creatLogFile function is called in the server constructor, and here we create the file if it is not there, and if it is, we use it and continue to write it.    
During the operation and activity of the server, the same few specific commands were mentioned (which was entering, creating, deleting, and downloading the file), in the case of the project, we write it in our `log.txt` file with the help of the addToLog function, and the process is like this: the command class `Command.cpp`, which processes the commands, checks the specific commands mentioned and prepares the output and gives it to the server in the form of a string, and the server writes to the file with the help of this function. First, it enters the time with the help of library functions and then enters the activity.    
Show the functions related to the loger:    
```cpp
Server* Server::instance = nullptr;

Server* Server::get_instance() {
    Configuration config = Configuration(PATH);
    if (instance == nullptr)
        instance = new Server(config);
    return instance;
}

 //just to make sure no other class instantiates Server
Server::Server(Configuration configuration) : config(configuration)
{
    
    commandPort = configuration.getCommandPort();
    dataPort = configuration.getDataPort();
    createLogFile();
}
```
```cpp
void Server::createLogFile()
{
    logFile.open(LOG_FILE_NAME, fstream::in | fstream::out | fstream::app);

    if (!logFile) 
    {
        cout << NO_LOG_FILE;
        logFile.open(LOG_FILE_NAME,  fstream::in | fstream::out | fstream::trunc);
        logFile.close();
    } 
}

```
```cpp
void Server::addToLog(string action)
{
    system_clock::time_point currentTime = system_clock::now();
    time_t cT = system_clock::to_time_t(currentTime);
    logFile.open(LOG_FILE_NAME, fstream::in | fstream::out | fstream::app);
     
    logFile << ctime(&cT);
    logFile << action;
    logFile.close();

}
```
Now we display the results of the commands stored in the logger:    
 ![s10-0](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s10-0.png "s10-0")    
Now we show the location of the file in the project folder:    
 ![s10-1](https://github.com/pooyaaf/CN_CHomeworks1/blob/0d42b52efb9e4f9592bf070c519333934d58d5f3/RFTP/IMG/s10-1.png "s10-1")    
Now we show the string output of the corresponding functions:    
 ![s10-2](https://github.com/pooyaaf/CN_CHomeworks1/blob/f0aca8daf53240b54e981a18ec7d31a67c8d2dfd/RFTP/IMG/s10-2.png "s10-2")    
Lines USER, PASS, MKD, DELE, RETR, UPLOAD are exactly the commands that we need to save the result in the logger. For this reason, they are of the string type that goes to the server through the command class and writes to the logger file through the named function.    
Now we show the part that is transferred from the data command to the logger function:    
```cpp
void Server::distinguishCommand(int socketFD, char* cmd)
{
    vector<string> wordsOfCmd = splitter(cmd);
    commands[socketFD]->setWordOfCommand(wordsOfCmd);
    CMDtype order = getCMDType(wordsOfCmd);
    
    switch (order)
    {
        case USER:
            addToLog(commands[socketFD]->loginUser(allClientsFD[socketFD].first));
            break;
        case PASS:
            addToLog(commands[socketFD]->checkPass(allClientsFD[socketFD].first));
            break;
        case PWD:
            commands[socketFD]->getCurrentDirectory();
            break;
        case MKD:
            addToLog(commands[socketFD]->makeNewDirectory());
            break;
        case DELE:
            addToLog(commands[socketFD]->delDirectory());
            break;
        case LS:
            commands[socketFD]->getFileList();
            break;
        case CWD:
            commands[socketFD]->changeDirectory(wordsOfCmd.size());
            break;
        case RENAME:
            commands[socketFD]->renameFile();
            break;
        case RETR:
            addToLog(commands[socketFD]->downloadFile());
            break;
        case UPLOAD:
            addToLog(commands[socketFD]->uploadFile());
            break;
        case HELP:
            commands[socketFD]->help();
            break;
        case QUIT:
            commands[socketFD]->quit();
            break;
        case ARGPAR_ERR:
            commands[socketFD]->setInvalActivity(PARA_SYNTAX_ERROR_A);
            break;
        case NOT_EXIST:
            commands[socketFD]->setInvalActivity(TOTAL_ERRORS_A);
            break;
    }
}

```
