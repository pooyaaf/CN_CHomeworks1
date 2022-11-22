#ifndef control
#define control

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// check file condition
bool have_access(vector<string> files, string file);
// get name of the file from client
string get_file_name(string path);
//  check user aceess controll
bool check_access(bool isAdmin, string target, vector<string> files);

#endif