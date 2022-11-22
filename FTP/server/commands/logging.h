#ifndef logging
#define logging

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

using namespace std;

// get user  current time
char *get_time();
// write to log.txt
void loggin(string log);
// command log create func
string make_log(string cmd, string name, string arg, bool success, string response);

#endif