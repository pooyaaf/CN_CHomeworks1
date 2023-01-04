#include "../library/Server.h"

using namespace std;
using namespace chrono;

Server* Server::instance = nullptr;

Server* Server::get_instance() {
    Configuration config = Configuration(PATH);
    if (instance == nullptr)
        instance = new Server(config);
    return instance;
}

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

 //just to make sure no other class instantiates Server
Server::Server(Configuration configuration) : config(configuration)
{
    
    commandPort = configuration.getCommandPort();
    dataPort = configuration.getDataPort();
    createLogFile();
}

void Server::addToLog(string action)
{
    system_clock::time_point currentTime = system_clock::now();
    time_t cT = system_clock::to_time_t(currentTime);
    logFile.open(LOG_FILE_NAME, fstream::in | fstream::out | fstream::app);
     
    logFile << ctime(&cT);
    logFile << action;
    logFile.close();

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

CMDtype userCmdErrHandel(int size)
{
    if(size == USER_CMD_COUNT)
        return USER;
    return ARGPAR_ERR;
}

CMDtype passCmdErrHandel(int size)
{
    if(size == PASS_CMD_COUNT)
        return PASS;
    return ARGPAR_ERR;
}

CMDtype pwdCmdErrHandel(int size)
{
    if(size == PWD_CMD_COUNT)
        return PWD;
    return ARGPAR_ERR;
}

CMDtype mkdCmdErrHandel(int size)
{
    if(size == MKD_CMD_COUNT)
        return MKD;
    return ARGPAR_ERR;
}

CMDtype deleCmdErrHandel(int size, string type)
{
    if((type == DELE_FILET)|(type == DELE_DIRT))
        if(size == DELE_CMD_COUNT)
            return DELE;
    return ARGPAR_ERR;
}

CMDtype lsCmdErrHandel(int size)
{
    if(size == LS_CMD_COUNT)
        return LS;
    return ARGPAR_ERR;
}

CMDtype cwdCmdErrHandel(int size)
{
    if((size == CWD_CMD_COUNT )|(size == CWD_CMD_COUNT-1))
        return CWD;
    return ARGPAR_ERR;
}

CMDtype renameCmdErrHandel(int size)
{
    if(size == RENAME_CMD_COUNT)
        return RENAME;
    return ARGPAR_ERR;
}

CMDtype retrCmdErrHandel(int size)
{
    if(size == RETR_CMD_COUNT)
        return RETR;
    return ARGPAR_ERR;
}

CMDtype uploadCmdErrHandel(int size)
{
    if(size == UPLOAD_CMD_COUNT)
        return UPLOAD;
    return ARGPAR_ERR;
}

CMDtype helpCmdErrHandel(int size)
{
    if(size == HELP_CMD_COUNT)
        return HELP;
    return ARGPAR_ERR;
}

CMDtype quitCmdErrHandel(int size)
{
    if(size == QUIT_CMD_COUNT)
        return QUIT;
    return ARGPAR_ERR;
}

CMDtype getCMDType(vector<string> wordsOfCmd)
{
    int size = wordsOfCmd.size();
    if (wordsOfCmd[INIT] == USER_CMD)
        return userCmdErrHandel(size);
    if (wordsOfCmd[INIT] == PASS_CMD)
        return passCmdErrHandel(size);
    if (wordsOfCmd[INIT] == PWD_CMD)
        return pwdCmdErrHandel(size);
    if (wordsOfCmd[INIT] == MKD_CMD)
        return mkdCmdErrHandel(size);
    if (wordsOfCmd[INIT] == DELE_CMD)
        return deleCmdErrHandel(size, wordsOfCmd[PARA1]);
    if (wordsOfCmd[INIT] == LS_CMD)
        return lsCmdErrHandel(size);
    if (wordsOfCmd[INIT] == CWD_CMD)
        return cwdCmdErrHandel(size);
    if (wordsOfCmd[INIT] == RENAME_CMD)
        return renameCmdErrHandel(size);
    if (wordsOfCmd[INIT] == RETR_CMD)
        return retrCmdErrHandel(size);
    if (wordsOfCmd[INIT] == UPLOAD_CMD)
        return uploadCmdErrHandel(size);
    if (wordsOfCmd[INIT] == HELP_CMD)
        return helpCmdErrHandel(size);
    if (wordsOfCmd[INIT] == QUIT_CMD)
        return quitCmdErrHandel(size);
    return NOT_EXIST;
}

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

int main(int argc, char * argv[])
{
    if(!Server::get_instance()->initiate())
        cout << "The server could not be built" << endl;

    return 0;
}
