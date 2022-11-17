#pragma once
#include <string>
#include <vector>
using namespace std;

struct Auth
{
    string name;
    string pass;
};

struct Configuration
{
    string baseDirectory = "none";
    vector<struct Auth> auth{
        {"Perriex", "moxeed"}};
};