#pragma once
#include "../context/AppContext.hpp"
#include "../Messages.h"
#include "logging.h"
#include "control.hpp"

// User command, used for sending password of a account to register.
void pass(AppContext *context, string arg, Configuration Configuration);
// User command, used for sending username of a account to register.
void user(AppContext *context, string arg, Configuration Configuration);
// User command, used for sending introductions to a user.
void help(AppContext *context, string arg, Configuration Configuration);
// User command, used for ending user system.
void quit(AppContext *context, string arg, Configuration Configuration);
// File system command, used for downloading a file from server if the user has access.
void retr(AppContext *context, string arg, Configuration Configuration);


typedef void (*command)(AppContext *context, string arg, Configuration Configuration);
