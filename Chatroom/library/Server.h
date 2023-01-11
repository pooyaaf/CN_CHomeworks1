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


typedef std::map<int, std::pair<int, int>> MP;

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

    static Server* instance;
};

#endif