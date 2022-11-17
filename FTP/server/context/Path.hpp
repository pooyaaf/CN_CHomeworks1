#include <string>

using namespace std;

class Path
{
private:
    string base;
    string current;
public:
    Path(string base_);
    string GetAbsolutePath();
    string GetVirtualPath();
    void Change(string relativepath);
};