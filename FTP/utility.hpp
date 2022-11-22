#ifndef UTILITY
#define UTILITY

#define BUF_SIZE 50

#include <string>
#include <netinet/in.h>

using namespace std;

void die(const char *scope);
void read_line(char *dst, int socket_fd);
int connect_to(int port);
void read_string(string *buffer, int fd);
void write_string(string buffer, int fd);

#endif