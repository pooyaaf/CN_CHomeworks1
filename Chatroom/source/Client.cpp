#include "../library/Client.h"

using namespace std;

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

    // while (true) 
    // {
    //     // Receive command from command line.
    //     cout << CMD_IN_SIGN;
    //     memset(cmd, 0, MAX_COMMAND_LENGTH);
    //     cin.getline(cmd, CMD_MAX_LEN);

    //     // Send command to server.
    //     send(clientCmdFileDes, cmd, CMD_MAX_LEN, 0);

    //     // Receive command output from server and show in command line.
    //     memset(outputCmd, 0, sizeof outputCmd);
    //     recv(clientCmdFileDes, outputCmd, sizeof(outputCmd), 0);
    //     cout << CMD_OUTPUT_COUT << outputCmd << endl;

    //     if (outputCmd == "Successful Quit.\n" )
    //         break;
        
    //     // Receive data output from server and show in command line.
    // }

    close(clientCmdFileDes);
    return true;
}

int main(int argc, char const *argv[])
{
    Client client;
    if(!client.initiate(atoi(argv[1])))
        cout << CLIENT_ERR << endl;

    return 0;
}
