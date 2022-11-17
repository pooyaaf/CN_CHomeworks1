#ifndef UTILITY
#define UTILITY

void die(char *scope);
void readLine(char *dst, int socket_fd);
int connectTo(int port);
int createBroadcastHub(int port, struct sockaddr_in *address, socklen_t size);

#endif