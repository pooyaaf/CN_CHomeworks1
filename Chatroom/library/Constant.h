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
#define TABER "  -"
#define PREV_DIR '.'
#define COLON ": "
#define SPACE " "
#define NEWL "\n"
#define DELIMITER " "
#define EMPTY ""
#define CMD_OUT_SIGN "<< "
#define CMD_IN_SIGN ">> "
#define BACK_DEL '\b'

#define PARA1 1
#define PARA2 2

#define INIT 0

#define NAME_IDX 1
#define ID_IDX 1
#define MESSAGE_IDX 2

#define CONNECT_CMD_COUNT 2
#define LIST_CMD_COUNT 1
#define INFO_CMD_COUNT 2
#define SEND_CMD_COUNT 3
#define RECEIVE_CMD_COUNT 1
#define QUIT_CMD_COUNT 1

#define CONNECT_CMD "connect"
#define LIST_CMD "list"
#define INFO_CMD "info"
#define SEND_CMD "send"
#define RECEIVE_CMD "receive"
#define EXIT_CMD "exit"

#define CONNECT_CMD_A "CONNECTACK"
#define LIST_CMD_A "LISTREPLAY"
#define INFO_CMD_A "USERINFOREPLAY"
#define SEND_CMD_A "SENDREPLAY"
#define STATUS_CMD_A "Status("
#define STATUS_SUCCESS_CMD_A ": success)"
#define STATUS_FAILURE_CMD_A ": failure)"
#define RECEIVE_CMD_A "RECEIVEREPLAY"

#define USER_SUCCESSFUL_LOGOUT_A "Successful Quit.\n"
#define NO_USER_FOR_CMD_A "Need account for login.\n"
#define PARA_SYNTAX_ERROR_A "Syntax error in parameters or arguments.\n"
#define TOTAL_ERRORS_A "Error, invalid Query.\n"

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