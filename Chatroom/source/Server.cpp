#include "../library/Server.h"

using namespace std;
using namespace chrono;

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

vector<string> splitter(char* entry)
{
    vector<string> words;
    char *ptr = strtok(entry,DELIMITER); 
   
    while (ptr != NULL) 
    {
        words.push_back(ptr);
        ptr = strtok(NULL,DELIMITER);
    }

    return words;
}

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

int acceptClient(int serverFd) 
{
    int clientFd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    clientFd = accept(serverFd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return clientFd;
}

string Server::getClientIDs()
{
    string res = "";
    for (int i = 0; i < lastClientIndex+1 ; i++)
        res += to_string(i) + DELIMITER;
    return res;
}

string Server::connect(int fd, string name)
{
    string res = EMPTY;
    res = CONNECT_CMD_A;
    res += DELIMITER;
    res += to_string(lastMessageIndex++);
    res += NEWL;
    cerr << "Server order.." << res << endl;
    clientsDB[allClients[socketFD]].first = wordsOfCmd[NAME_IDX];
    allClientsFD[allClients[socketFD]].second = 1;
    return res;
}

string Server::list()
{
    string res = EMPTY;
    return res;

}

string Server::info()
{
    string res = EMPTY;
    return res;

}

string Server::send()
{
    string res = EMPTY;
    return res;

}

string Server::receive()
{
    string res = EMPTY;
    return res;

}

CMDtype connectCmdErrHandel(int size)
{
    if(size == CONNECT_CMD_COUNT)
        return CONNECT;
    return ARGPAR_ERR;
}

CMDtype listCmdErrHandel(int size)
{
    if (size == LIST_CMD_COUNT)
        return LIST;
    return ARGPAR_ERR;
}

CMDtype infoCmdErrHandel(int size)
{
    if (size == INFO_CMD_COUNT)
        return INFO;
    return ARGPAR_ERR;
}

CMDtype sendCmdErrHandel(int size)
{
    if (size == SEND_CMD_COUNT)
        return SEND;
    return ARGPAR_ERR;
}

CMDtype receiveCmdErrHandel(int size)
{
    if (size == RECEIVE_CMD_COUNT)
        return RECEIVE;
    return ARGPAR_ERR;
}

CMDtype getCMDType(vector<string> wordsOfCmd)
{
    int size = wordsOfCmd.size();
    if (wordsOfCmd[INIT] == CONNECT_CMD)
        return connectCmdErrHandel(size);
    if (wordsOfCmd[INIT] == LIST_CMD)
        return listCmdErrHandel(size);
    if (wordsOfCmd[INIT] == INFO_CMD)
        return infoCmdErrHandel(size);
    if (wordsOfCmd[INIT] == SEND_CMD)
        return sendCmdErrHandel(size);
    if (wordsOfCmd[INIT] == RECEIVE_CMD)
        return receiveCmdErrHandel(size);
    return NOT_EXIST;
}

void Server::distinguishCommand(int socketFD, char* cmd)
{
    vector<string> wordsOfCmd = splitter(cmd);
    CMDtype order = getCMDType(wordsOfCmd);
    int id = -1, status = -1;
    string message = "";
    
    switch (order)
    {
        case CONNECT:
            break;
        case LIST:
            commands[socketFD] = LIST_CMD_A;
            commands[socketFD] += DELIMITER;
            commands[socketFD] += to_string(lastMessageIndex++);
            commands[socketFD] += NEWL;
            commands[socketFD] += getClientIDs();
            commands[socketFD] += NEWL;
            break;
        case INFO:
            commands[socketFD] = INFO_CMD_A;
            commands[socketFD] += DELIMITER;
            commands[socketFD] += to_string(lastMessageIndex++);
            commands[socketFD] += NEWL;
            id = stoi(wordsOfCmd[ID_IDX]);
            commands[socketFD] += id > lastClientIndex ? EMPTY : clientsDB[id].first;
            commands[socketFD] += NEWL;
            break;
        case SEND:
            commands[socketFD] = SEND_CMD_A;
            commands[socketFD] += DELIMITER;
            commands[socketFD] += to_string(lastMessageIndex++);
            commands[socketFD] += NEWL;
            id = stoi(wordsOfCmd[ID_IDX]);
            message = to_string(allClients[socketFD]) + DELIMITER + wordsOfCmd[MESSAGE_IDX] + NEWL;
            clientsDB[id].second += message;
            status = allClientsFD[id].second;
            commands[socketFD] += STATUS_CMD_A;
            commands[socketFD] += to_string(status);
            commands[socketFD] += status ? STATUS_SUCCESS_CMD_A : STATUS_FAILURE_CMD_A ;
            commands[socketFD] += NEWL;
            break;
        case RECEIVE:
            commands[socketFD] = RECEIVE_CMD_A;
            commands[socketFD] += DELIMITER;
            commands[socketFD] += to_string(lastMessageIndex++);
            commands[socketFD] += NEWL;
            commands[socketFD] += clientsDB[allClients[socketFD]].second;
            break;
        case ARGPAR_ERR:
            commands[socketFD] = "";
            break;
        case NOT_EXIST:
            commands[socketFD] = "";
            break;
    }
}

bool Server::initiate()
{
    int serverFd, newSocket; 
    char buffer[1024] = {0};

    
    fd_set master_set, working_set;
    
    serverFd = setupServer(SERVEER_PORT);

    if (serverFd < 0)
        return 0;

    cerr << "server fd: " << serverFd << endl;

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

                    allClientsFD[lastClientIndex] = make_pair(newSocket, NOT_CONNECTED);
                    clientsDB.push_back(make_pair(EMPTY, EMPTY));
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
                    isConnected = allClientsFD[allClients[i]].second;

                    if (bytes_received > 0) 
                    {
                        distinguishCommand(allClients[i], buffer);
                        
                        cerr << "----------------------" << NEWL <<(commands[allClients[i]]) << "----------------------" << NEWL ;
                        //cmd
                        send(allClientsFD[allClients[i]].first, 
                            (commands[allClients[i]].c_str()), 
                            strlen(commands[allClients[i]].c_str()), 0);
                        //data
                    }
                    
                    if ((bytes_received == 0)||(isConnected == 0))
                    { // EOF
                        cerr << "Client closed with fd: " << i << endl;

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

int main(int argc, char * argv[])
{
    if(!Server::get_instance()->initiate())
        cout << "The server could not be built" << endl;

    return 0;
}
