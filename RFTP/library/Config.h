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