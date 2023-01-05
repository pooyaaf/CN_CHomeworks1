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
