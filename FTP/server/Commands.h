
// Download a file
#define RETR "retr"
// Upload a file
#define UPLOAD "Uploadgit "
// Help
#define HELP "help"

// Quit
#define QUIT "quit"

// Commands - sample

#define USER_COMMAND 0
#define PASS_COMMAND 1
#define RETR_COMMAND 2
#define UPLOAD_COMMAND 3
#define HELP_COMMAND 4
#define QUIT_COMMAND 5
// test
#define COMMAND(NAME)  { #NAME,  ## _command }

struct command
{
  char *name;
  void (*function) (void);
};

struct command commands[] =
{
  COMMAND (user),     // 0
  COMMAND (pass),     // 1
  COMMAND (retr),     // 3
  COMMAND (upload),   // 4
  COMMAND (help),     // 5
  COMMAND (quit),     // 5
};

void user_command();
void pass_command();
void retr_command();
void upload_command();
void help_command();
void quit_command();