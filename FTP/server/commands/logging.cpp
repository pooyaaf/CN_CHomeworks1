#include "logging.h"

using namespace std;

char *get_time()
{
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    return ctime(&end_time);
}

void loggin(string log)
{
    string filename("log.txt");
    ofstream file_out;

    file_out.open(filename, std::ios_base::app);
    file_out << log << ". Time : " << get_time();
    file_out.close();
}

string make_log(string cmd, string name, string arg, bool success, string response)
{
    string title = name != "" ? "User \"" + name + "\"" : "Unkown User";
    string log = "command #" + cmd + arg != "" ? " with argument \"" + arg + "\"" : "without argument";
    string status = success == true ? "succeed" : "faild";

    return title + ": " + log + ". status: " + status + " with response : " + response;
}