#include "../library/Config.h"

using namespace std;
using namespace boost::property_tree;

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

string Configuration::getOrgAddr()
{
    return orgAddr;
}

int Configuration::getCommandPort() 
{
    return commandPort;
}

int Configuration::getDataPort() 
{
    return dataPort;
}

std::vector<User*> Configuration::getUser() 
{
    return users;
}

std::vector<std::string> Configuration::getFile() 
{
    return files;
}