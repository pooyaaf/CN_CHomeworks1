#include "control.hpp"

bool have_access(vector<string> files, string file)
{
    for (int i = 0; i < files.size(); i++)
    {
        if (file == files[i])
            return false;
    }
    return true;
}

string get_file_name(string path)
{
    int index = path.find_last_of('/');

    if (index < path.size())
        return path.substr(index + 1);

    return path;
}

bool check_access(bool isAdmin, string target, vector<string> files)
{
    return isAdmin == true && have_access(files, target) == true;
}