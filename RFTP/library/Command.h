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