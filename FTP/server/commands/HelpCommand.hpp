#include "Command.hpp"
#include "../context/AppContext.hpp"
#include "unistd.h"

class HelpCommand : Command
{
public:
    void Exectute(AppContext context) override;
};

void HelpCommand::Exectute(AppContext context)
{
    fstream help_file;

    string help;
    string line;

    help_file.open("help.txt", ios::in);
    if (!help_file)
    {
        cout << "No such file";
    }
    else
    {
        while (getline(help_file, line))
            help += line + "\n";
    }
    help_file.close();

    write(context.commandFd, help.c_str(), help.size());
}