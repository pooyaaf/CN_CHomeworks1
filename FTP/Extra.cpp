#include "Extra.h"

using namespace std;


vector<string> read_help()
{
    string line;
    vector<string> help;

    ifstream help_file("help.txt");

    while (getline(help_file, line))
    {
        help.push_back(line);
    }

    help_file.close();

    return help;
} 

char* getTime()
{
    auto start = chrono::system_clock::now();
    auto end = chrono::system_clock::now();

    chrono::duration<double> elapsed_seconds = end-start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    return ctime(&end_time);
}


void loggin(string log)
{
    ofstream fout;  
    ifstream fin;

    fin.open("log.txt");
    fout.open ("log.txt",ios::app); 

    if(fin.is_open())
        fout<< log << " - done at :" << getTime() << "\n" ; 

    fin.close();
    fout.close(); 
}