#ifndef _FTP_H_
#define _FTP_H_

#include "Macros.h"

typedef struct {
    char* user;
    char* password;
    char* hostName;
    char* path;
    char* fileName;
} FTPConnection;

FTPConnection FTPConn;

int FTPSendRec(int sockfd, char* buf, char* response);

int FTPOpen();

int FTPLogin();

int FTPSetPassiveMode();

int FTPRetrieve();

int FTPCloseConnection();

#endif
