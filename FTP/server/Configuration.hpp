#pragma once
#include <string>
#include <vector>
using namespace std;

struct Auth
{
    string name;
    string pass;
    bool admin;
    long size;
};

struct Configuration
{
    string baseDirectory = "root";
    int command_channel_port;
    int data_channel_port;
    vector<struct Auth> auth;
    vector<string> admin_files;
};