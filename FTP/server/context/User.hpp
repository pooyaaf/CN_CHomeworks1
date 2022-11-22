#pragma once
#include <string>

using namespace std;

class User
{
private:
    string name;
    bool registered = false;
    bool isAdmin;
    long int size;

public:
    long int used_size;
    User(string name_, bool isAdmin_, long int _size)
    {
        name = name_;
        isAdmin = isAdmin_;
        size = _size;
        registered = false;
        used_size = 0;
    }
    void setRegistered()
    {
        registered = true;
    }
    bool isRegistered()
    {
        return registered;
    }
    long get_size_limit()
    {
        return size;
    }
    bool isUserAdmin()
    {
        return isAdmin;
    }
    string get_name(){
        return name;
    }
};
