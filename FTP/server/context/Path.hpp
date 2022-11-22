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
    string GetAbsolutePath(string path);
    string GetVirtualPath();
    string GetVirtualPath(string path);
    void Change(string relativepath);
};
