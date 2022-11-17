// Login messages
#define LOGIN_USER_SUCCESS "331: User name okay, need password."
#define LOGIN_PASS_ERROR "503: Bad sequence of commands."
#define LOGIN_PASS_SUCCESS "230: User logged in, proceed. Logged out if appropriate."
#define LOGIN_INVALID_ERROR "430: Invalid username or password"

// Access control
#define FILE_ACCESS_ERROR "550: File unavailable."

// Download / Upload a file
#define RETR_SUCCESS "226: Successful Download."
#define RETR_ERROR "425: Can't open data connection."

// Help
#define HELP_RETURN "214"

// ! Errors
#define AUTH_ERROR "332: Need account for login."
#define SYNTAX_ERROR "501: Syntax error in parameters or arguments."
#define GENERAL_ERROR "500: Error"

// Quit
#define QUIT_SUCCESS "221: Successful Quit."