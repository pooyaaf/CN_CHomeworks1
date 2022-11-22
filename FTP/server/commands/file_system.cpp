#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <iostream>
#include <experimental/filesystem>

#include "Commands.hpp"
#include "../../utility.hpp"



void retr(AppContext *context, string arg, Configuration configuration)
{
    string target = context->path.GetAbsolutePath(arg);

    struct stat state_buf;
    int rc = stat(target.c_str(), &state_buf);

    if (!check_access(context->user->isUserAdmin(), arg, context->files))
    {
        write_string(FILE_ACCESS_ERROR, context->commandFd);
        loggin(make_log(RETR, context->user->get_name(), arg, false, FILE_ACCESS_ERROR));
        return;
    }

    if (rc != 0)
    {
        write_string(GENERAL_ERROR, context->commandFd);
        loggin(make_log(RETR, context->user->get_name(), arg, false, GENERAL_ERROR));
        return;
    }

    if (state_buf.st_size + context->user->used_size > context->user->get_size_limit())
    {
        write_string(RETR_ERROR, context->commandFd);
        loggin(make_log(RETR, context->user->get_name(), arg, false, RETR_ERROR));
        return;
    }

    write_string(get_file_name(target), context->dataFd);
    write(context->dataFd, &state_buf.st_size, sizeof(off_t));

    int read_count;
    char buf[50];

    int file = open(target.c_str(), O_RDONLY, 0777);
    while ((read_count = read(file, buf, sizeof buf)) > 0)
    {
        write(context->dataFd, buf, read_count);
    }

    context->user->used_size += state_buf.st_size;
    write_string(RETR_SUCCESS, context->commandFd);
    loggin(make_log(RETR, context->user->get_name(), arg, true, RETR_SUCCESS));
}

