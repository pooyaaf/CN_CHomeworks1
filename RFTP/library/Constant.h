#ifndef CONSTANT_H_
#define CONSTANT_H_ "CONSTANT_H"

#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#define COMMAND 0
#define SUCCESS 0

#define PATH "config.json"

#define BACK_SLASH "/"
#define DIR '/'
#define PREV_DIR '.'
#define COLON ": "
#define SPACE " "
#define NEWL "\n"

#define DUPLICATE "d_"
#define SERVER_FILE_DIRECTORY "RFTP/Files/"
#define UPLOAD_DIR "RFTP"
#define UPLOAD_DIR_LEN 4
#define UPLOAD_LIMIT 1024000

#define DELIMITER " "
#define EMPTY " "
#define PARA1 1
#define PARA2 2

#define CWD_NO_PARA 1
#define INIT 0
#define USER_CMD_COUNT 2
#define PASS_CMD_COUNT 2
#define PWD_CMD_COUNT 1
#define MKD_CMD_COUNT 2
#define DELE_CMD_COUNT 3
#define LS_CMD_COUNT 1
#define CWD_CMD_COUNT 2
#define RENAME_CMD_COUNT 3
#define RETR_CMD_COUNT 2
#define UPLOAD_CMD_COUNT 2
#define HELP_CMD_COUNT 1
#define QUIT_CMD_COUNT 1

#define USER_CMD "user"
#define PASS_CMD "pass"
#define PWD_CMD "pwd"
#define MKD_CMD "mkd"
#define DELE_CMD "dele"
#define LS_CMD "ls"
#define CWD_CMD "cwd"
#define RENAME_CMD "rename"
#define RETR_CMD "retr"
#define UPLOAD_CMD "Upload"
#define HELP_CMD "help"
#define QUIT_CMD "quit"

#define DELE_FILET "-f"
#define DELE_DIRT "-d"

#define USER_ACCEPTED_A "331: User name okay, need password."
#define CLIENT_NOT_LOG_A "503: Bad sequence of commands."
#define USER_SUCCESSFUL_LOGIN_A "230: User logged in, proceed. Logged out if appropriate."
#define INVALID_USER_PASS_A "430: Invalid username or password"
#define FILE_ACCESS "550: File unavailable."
#define PWD_A "257: "
#define MAKE_A "257: "
#define CREATE_A "created."
#define DELETE_A "250: "
#define DELETED_A "deleted."
#define LIST_TRANSFER_A "226: List transfer done."
#define SUCCESSFUL_CHANGE_A "250: Successful change."
#define SUCCESSFUL_DOWNLOAD_A "226: Successful Download."
#define HELP_TAG_A "214\n"
#define USER_SUCCESSFUL_LOGOUT_A "221: Successful Quit."
#define NO_USER_FOR_CMD_A "332: Need account for login."
#define PARA_SYNTAX_ERROR_A "501: Syntax error in parameters or arguments."
#define TOTAL_ERRORS_A "500: Error"
#define DOWNLOAD_LIMIT_A "425: Can't open data connection."

#define USER_HELP_DEC "USER [name], Its argument is used to specify the user's name. It is used for user authentication.\n"
#define PASS_HELP_DEC "PASS [password], Its argument is used to specify the user's password. It is used for user authentication.\n"
#define PWD_HELP_DEC "PWD, It is used to print the current working directory path.\n"
#define MKD_HELP_DEC "MKD [path], Its argument is used to specify the directory's path. It is used to create a new file(using types) or directory(using /).\n"
#define DELE_HELP_DEC "DELE [flag] [path], Its argument are used to specify the file(-f)/directory(-d)'s path. It flag is used to specify the deleting type file(-f)/directory(-d). It is used to delete a file or directory.\n"
#define LS_HELP_DEC "LS. It is used to print the list of files/directories in the current working directory path.\n"
#define CWD_HELP_DEC "CWD [path], Its argument is used to specify the directory's path. It is used to change the current working directory path to entry working directory path.\n"
#define RENAME_HELP_DEC "RENAME [from] [to], Its arguments are used to specify the current and future file's name. It is used to change a file's name.\n"
#define RETR_HELP_DEC "RETR [name], Its argument is used to specify the file's name. It is used to download a file.\n"
#define UPLOAD_HELP_DEC "UPLOAD [name], Its argument is used to specify the file's name. It is used to upload a file in server.\n"
#define HELP_HELP_DEC "HELP, It is used to show instructions guide.\n"
#define QUIT_HELP_DEC "QUIT, It is used for current user to sign out from the server.\n"

enum CMDtype
{
    USER,
    PASS,
    PWD,
    MKD,
    DELE,
    LS,
    CWD,
    RENAME,
    RETR,
    UPLOAD,
    HELP,
    QUIT,
    ARGPAR_ERR,
    NOT_EXIST
};

#endif