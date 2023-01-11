#include "../library/Command.h"

using namespace std;

Command::Command(std::vector<User*> users_, std::vector<std::string> files_, std::string addr_)
{
    users = users_;
    files = files_;
    onlineUser = -1;
    currentDirectory = addr_ ;
    currentDirectory += BACK_SLASH;
}

void Command::setWordOfCommand(vector<string> wordsOfCmd_)
{
    wordsOfCmd = wordsOfCmd_;
}

void Command::setDirectory(string newDir)
{
    currentDirectory =  newDir;
}

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

void Command::getCurrentDirectory()
{
    resetChannels();
    if(isLogin())
        cmdChannel = PWD_A + currentDirectory;

}

string Command::makeNewDirectory()
{
    resetChannels();
    string action = "";
    if(isLogin())
    {
        string newDirectory = "mkdir " + currentDirectory + wordsOfCmd[PARA1];
        int status = system(newDirectory.c_str());
        if (status == SUCCESS) 
        {
            cmdChannel = MAKE_A + wordsOfCmd[PARA1] + SPACE + CREATE_A;
            action += (users[onlineUser]->getName() + COLON + 
                    "Has successfully create directory named " + wordsOfCmd[PARA1] + 
                    " in " + currentDirectory + "." + NEWL);
        }
        else
        {
            cmdChannel = TOTAL_ERRORS_A;
            action += (users[onlineUser]->getName() + COLON + 
                    "Hasn't create directory named " + wordsOfCmd[PARA1] + 
                    " in " + currentDirectory + "." + NEWL);
        }
    }
    return action;
}

string Command::delDirectory()
{
    resetChannels();
    string action = "";
    if(isLogin())
    {
        if(!wordsOfCmd[PARA1].compare(DELE_DIRT))
        {
            string delCommand = "rm -r " + currentDirectory + wordsOfCmd[PARA2];
            int status = system(delCommand.c_str());
            if (status == SUCCESS) 
            {
                cmdChannel = DELETE_A + wordsOfCmd[PARA2] + SPACE + DELETED_A;
                action += (users[onlineUser]->getName() + COLON + 
                    "Has successfully deleted directory named " + wordsOfCmd[PARA2] + 
                    " in " + currentDirectory + "." + NEWL);
            }
        }
        else if(!wordsOfCmd[PARA1].compare(DELE_FILET))
        {
            if (!users[onlineUser]->getIsAdmin() && fileIsSecure(wordsOfCmd[PARA1]))
            {
                cmdChannel = FILE_ACCESS;
                action += (users[onlineUser]->getName() + COLON + 
                    "Permission denied on file named " + wordsOfCmd[PARA2] + "." + NEWL);
                return action;
            }
            string bash_command = "rm " + currentDirectory + wordsOfCmd[PARA2];
            int status = system(bash_command.c_str());
            if (status == SUCCESS) 
            {    
                cmdChannel = DELETE_A + wordsOfCmd[PARA2] + SPACE + DELETED_A;
                action += (users[onlineUser]->getName() + COLON + 
                    "Has successfully deleted file named " + wordsOfCmd[PARA2] + 
                    " in " + currentDirectory + "." + NEWL);
            }
            else
            {
                cmdChannel = TOTAL_ERRORS_A;
                action += (users[onlineUser]->getName() + COLON + 
                    "Hasn't successfully deleted file named " + wordsOfCmd[PARA2] + 
                    " in " + currentDirectory + "." + NEWL);    
            }
        }
    }
    return action;
}

string toStringConverter(string fileN)
{
    ifstream File(fileN, ios::binary);
    string temp;
    string whole = "";
    while (getline(File,temp,'\n'))
    {
        whole += temp;
        whole += NEWL;
    }  
    return whole;
}

void Command::getFileList()
{
    resetChannels();
    if(isLogin())
    {
        string lsCommand = "ls " + currentDirectory + " > filelist.txt";
        int status = system(lsCommand.c_str());
        if (status == SUCCESS)
        {
            cmdChannel = LIST_TRANSFER_A;
            dataChannel = NEWL;
            dataChannel += toStringConverter("filelist.txt");
            int status_ = system("rm filelist.txt");
            if (status != SUCCESS)
                cmdChannel = TOTAL_ERRORS_A;
        }
        else
            cmdChannel = TOTAL_ERRORS_A;
    }
}

void Command::changeDirectory(int isOrg)
{
    resetChannels();
    if(isLogin())
    {
        string cmd;
        if (isOrg == CWD_NO_PARA)
            cmd = "realpath " + users[onlineUser]->getOrgAddr() + "> temp.txt";
        else
            cmd = "realpath " + currentDirectory + wordsOfCmd[PARA1] + "> temp.txt";
        int status = system(cmd.c_str());
        if (status == SUCCESS)
        {
            ifstream myFile("temp.txt");
            string addr;
            getline(myFile, addr);
            myFile.close();
            string delCommand = "rm -r temp.txt";
            int status = system(delCommand.c_str());

            setDirectory(addr + BACK_SLASH);
            // cout << addr << " 111 " << endl;
            cmdChannel = SUCCESSFUL_CHANGE_A;
        }
        else
            cmdChannel = TOTAL_ERRORS_A;
    }
}

void Command::renameFile()
{
    resetChannels();
    if(isLogin())
    {
        if (!users[onlineUser]->getIsAdmin() && fileIsSecure(wordsOfCmd[PARA1]))
        {
            cmdChannel = FILE_ACCESS;
            return;
        }
        string renameCommand = "mv " + currentDirectory + wordsOfCmd[PARA1] + " " +
                currentDirectory + wordsOfCmd[PARA2];
        int status = system(renameCommand.c_str());
        if (status == SUCCESS)
            cmdChannel = SUCCESSFUL_CHANGE_A;
        else
            cmdChannel = TOTAL_ERRORS_A;
    }
}

string extractFileName(string addr)
{
    size_t pos = addr.find_last_of(BACK_SLASH);
    if (pos == string::npos)
        return addr;

    string res = addr.substr(pos+1, addr.size());
    return res;
}

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

bool Command::fileIsSecure(string fileName)
{
    for(int i = 0 ; i < files.size() ; i++)
    {
        if(files[i] == fileName)
            return true;
    } 
    return false;
}

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

string getServerDirectory(string currDir)
{
    int mainScope = currDir.find(UPLOAD_DIR);
    string res = currDir.substr(0, mainScope);
    res += SERVER_FILE_DIRECTORY;
    if (mkdir(res.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
        if( errno == EEXIST ); 
        else ;

    // cerr << "server dest: " << res << "\n";
    return res;
}

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

void Command::setUserQuit()
{
    for(int i = 0 ; i < users.size() ; i++)
        users[i]->setUserStatus(NOT_ENTERED);
    onlineUser = -1;
}

std::string Command::getDataChannel()
{
    return dataChannel;
}

std::string Command::getCmdChannel()
{
    return cmdChannel;
}

void Command::resetChannels()
{
    cmdChannel.clear();
    dataChannel.clear();

    cmdChannel = EMPTY;
    dataChannel = EMPTY;
}

void Command::setInvalActivity(string activity)
{
    resetChannels();
    cmdChannel = activity;
    dataChannel = EMPTY;
}