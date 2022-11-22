#pragma once
#include "../context/AppContext.hpp"
#include "../../Messages.h"

void pass(AppContext* context, string arg, Configuration Configuration);
void user(AppContext* context, string arg, Configuration Configuration);
void help(AppContext* context, string arg, Configuration Configuration);
void quit(AppContext* context, string arg, Configuration Configuration);

typedef void(*command)(AppContext* context, string arg, Configuration Configuration);
