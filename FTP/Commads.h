// Login commands
#define USER "user"
#define PASS "pass"

// Download a file
#define RETR "retr"

// Help
#define HELP "help"

// Quit
#define QUIT "quit"


// Commands
// just a sample

#define USER_COMMAND 0
#define PASS_COMMAND 1
#define RETR_COMMAND 2
#define HELP_COMMAND 3
#define QUIT_COMMAND 4

#define COMMAND(NAME)  { #NAME, NAME ## _command }

struct command
{
  char *name;
  void (*function) (void);
};

struct command commands[] =
{
  COMMAND (user),     // 0
  COMMAND (pass),     // 1
  COMMAND (retr),     // 2
  COMMAND (help),     // 3
  COMMAND (quit),     // 4
};

void user_command();
void pass_command();
void retr_command();
void help_command();
void quit_command();
