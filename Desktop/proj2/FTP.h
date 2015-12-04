#ifndef _FTP_H_
#define _FTP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sockets.h"

#define FTP_PORT 21
#define MAX_CTRL_LENGTH 512
#define MAX_DATA_LENGTH 1024
#define MAX_USER_SIZE 128
#define MAX_PASS_SIZE 128
#define MAX_HOST_SIZE 128
#define MAX_PATH_SIZE 512
#define FTP_RESP_LENGTH 3
#define IP_PASV_LENGTH 4
#define PORT_PASV_LENGTH 2
#define IP_LENGTH 16

typedef struct
{
    char* user;
    char* pass;
    char* hostname;
    char* path;
    char* filename;

    int ctrlSocket;
    int dataSocket;
} FTPConnection;

FTPConnection conn;

int FTPSendRec(int sockfd, char* buf, char* response);	

int FTPOpenConnection();

int FTPLogin();

int FTPSetPassiveMode();

int FTPRetrieve();

int FTPCloseConnection();

#endif
