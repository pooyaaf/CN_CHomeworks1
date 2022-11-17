#include<string>

using namespace std;

class User
{
private:
    string name;
    bool isAdmin;
public:
    User(string name_, bool isAdmin_);
};

User::User(string name_, bool isAdmin_)
{
    name = name_;
    isAdmin = isAdmin_;
}