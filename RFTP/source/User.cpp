#include "../library/User.h"

using namespace std;

User::User(string username_, string pass_, bool isAdmin_, int downladSize_, string orgAddress_)
{
    username = username_;
    pass = pass_;
    isAdmin = isAdmin_;
    downladSize = downladSize_;
    userStatus = NOT_ENTERED;
    orgAddress = orgAddress_;
}

string User::getName()
{
    return username;
}

string User::getPass()
{
    return pass;
}

bool User::getIsAdmin()
{
    return isAdmin;
}

int User::getDSize()
{
    return downladSize;
}

int User::getStatus()
{
    return userStatus;
}

void  User::setUserStatus(int status)
{
    userStatus = status;
}

void User::decDownloadSize(int decSize)
{
    downladSize -= decSize;
}

bool User::isAbleToDownload(int decSize)
{
    if(downladSize - decSize > 0)
        return true;
    return false;
}

string User::getOrgAddr()
{
    return orgAddress;
}

void User::setClientID(int clientID_)
{
    clientID = clientID_;
}

int User::getClientID()
{
    return clientID;
}