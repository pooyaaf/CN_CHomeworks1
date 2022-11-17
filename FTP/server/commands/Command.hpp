#include "../context/AppContext.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

class Command
{
public:
    virtual void Exectute(AppContext context) = 0;
};