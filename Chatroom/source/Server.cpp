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
    if (size >= SEND_CMD_COUNT)
        return SEND;
    return ARGPAR_ERR;
}

CMDtype receiveCmdErrHandel(int size)
{
    if (size == RECEIVE_CMD_COUNT)
        return RECEIVE;
    return ARGPAR_ERR;
}

CMDtype exitCmdErrHandel(int size)
{
    if (size == QUIT_CMD_COUNT)
        return QUIT;
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
    if (wordsOfCmd[INIT] == EXIT_CMD)
        return exitCmdErrHandel(size);
    return NOT_EXIST;
}

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

int main(int argc, char * argv[])
{
    if(!Server::get_instance()->initiate())
        cout << "The server could not be built" << endl;

    return 0;
}
