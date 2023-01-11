#ifndef CONSTANT_H_
#define CONSTANT_H_ "CONSTANT_H"

#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#define SERVEER_PORT 8080
#define NOT_CONNECTED 0

#define COMMAND 0
#define SUCCESS 0

#define BACK_SLASH "/"
#define DIR '/'
#define PREV_DIR '.'
#define COLON ": "
#define SPACE " "
#define NEWL "\n"
#define DELIMITER " "
#define EMPTY ""

#define PARA1 1
#define PARA2 2

#define INIT 0

#define NAME_IDX 2
#define ID_IDX 2
#define MESSAGE_IDX 3

#define CONNECT_CMD_COUNT 3
#define LIST_CMD_COUNT 2
#define INFO_CMD_COUNT 3
#define SEND_CMD_COUNT 4
#define RECEIVE_CMD_COUNT 2
#define QUIT_CMD_COUNT 0

#define CONNECT_CMD "connect"
#define LIST_CMD "list"
#define INFO_CMD "info"
#define SEND_CMD "send"
#define RECEIVE_CMD "receive"

#define CONNECT_CMD_A "CONNECTACK"
#define LIST_CMD_A "LISTREPLAY"
#define INFO_CMD_A "USERINFOREPLAY"
#define SEND_CMD_A "SENDREPLAY"
#define STATUS_CMD_A "Status("
#define STATUS_SUCCESS_CMD_A ": success)"
#define STATUS_FAILURE_CMD_A ": failure)"
#define RECEIVE_CMD_A "RECEIVEREPLAY"

#define USER_SUCCESSFUL_LOGOUT_A "Successful Quit."
#define NO_USER_FOR_CMD_A "Need account for login."
#define PARA_SYNTAX_ERROR_A "Syntax error in parameters or arguments."
#define TOTAL_ERRORS_A "Error"

enum CMDtype
{
    CONNECT,
    LIST,
    INFO,
    SEND,
    RECEIVE,
    QUIT,
    ARGPAR_ERR,
    NOT_EXIST
};

#endif