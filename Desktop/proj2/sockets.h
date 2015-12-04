#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OK 0
#define ERROR -1

int socketConnection(char* hostname, int port);

int socketRead(int sockfd, char* buf, int lenght);

int socketWrite(int sockfd, char* buf, int lenght);

#endif
