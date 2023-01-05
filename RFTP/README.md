# CN_CA1_P1
computer networking projects Fall 1401 - CA1_part1:    
GOAL: learning basic concepts of Socket programming,    
and how to design program and their relations so that we implement 3 phases with one Local Host and several Ports.    
FTP server: simple FTP server with 2 Client & Server parts, in order to control downloading and uploading files with a certain size(1024kbytes).
___
This program is implemented in object oriented c++ programming. For each entity in the program, separate classes are considered as follows:    

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
