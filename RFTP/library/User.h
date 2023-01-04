#ifndef USER_H_
#define USER_H_ "USER_H"

#include <string>

#define NOT_ENTERED 0
#define WAIT_FOR_PASS 1
#define ENTERED 2

class User
{
public:
    User(std::string username_, std::string pass_, bool isAdmin_, 
            int downladSize_, std::string orgAddress_);
    std::string getName();
    std::string getPass();
    bool getIsAdmin();
    int getDSize();
    int getStatus();
    void setUserStatus(int status);
    void decDownloadSize(int decSize);
    bool isAbleToDownload(int decSize);
    std::string getOrgAddr();
    void setClientID(int clientID_);
    int getClientID();

private:
    std::string orgAddress;
    std::string username;
    std::string pass;
    bool isAdmin;
    int downladSize;
    int userStatus;
    int clientID;
};


#endif