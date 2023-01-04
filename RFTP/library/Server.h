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