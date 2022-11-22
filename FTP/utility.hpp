#ifndef UTILITY
#define UTILITY

#include <string>
#include <netinet/in.h>

using namespace std;

void die(const char *scope);
void readLine(char *dst, int socket_fd);
int connectTo(int port);
int createBroadcastHub(int port, struct sockaddr_in *address, socklen_t size);
void read_string(string* buffer, int fd);
void write_string(string& buffer, int fd);

#endif