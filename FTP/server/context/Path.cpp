#include"Path.hpp"
#include <sys/stat.h>
#include<iostream>

bool IsPathExist(const std::string &s)
{
    struct stat buffer;
    return (stat (s.c_str(), &buffer) == 0);
}

Path::Path(string base_){
    base= base_;
    current = "";
}

string Path::GetAbsolutePath(){
    return base + "/" + current;
}

string Path::GetAbsolutePath(string path){
    string middle = current == "" ? "" : current + "/";
    return base + "/" + middle + path;
}

string Path::GetVirtualPath(){
    return "/" + current;
}

string Path::GetVirtualPath(string path){
    string middle = current == "" ? "" : current + "/";
    return middle + path;
}

void Path::Change(string relativepath){

    if(relativepath.size() == 0)
        return;

    string segment;
    int i = 0;
    for (;i < relativepath.size(); i++)
    {
        if(relativepath[i] == '/')
        {
            i++;
            break;
        }
        segment += relativepath[i];
    }
    if(segment == "")
    {
        current = "";
    }
    if(segment == "..")
    {
        int index = current.find_last_of("/");
        if(index < current.size() && current != "") {
            current = current.substr(0, index);
        }
        else {
            current = "";
        }
    }
    else
    {
        string middle = current == "" ? "" : current + "/";
        if(IsPathExist(base + "/" + middle + segment))
        {
            current = middle + segment;
        }
        else
        {
            throw invalid_argument("path not found");
        }
    }

    Change(relativepath.substr(i));
}