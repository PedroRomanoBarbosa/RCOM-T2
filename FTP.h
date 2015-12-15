#ifndef _FTP_H_
#define _FTP_H_

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parser.h"
#include "macros.h"

#define FTP_PORT 21

struct url{
	int anon;
	char* username;
	char* password;
	char* host;
	char* filePath;
} FTPUrl;

int initializeStruct(char* matches[], int size);

int printStruct();

int sendRec(int sockfd, char* buf, char* response);

int openConnection();

int parseUrl(char* str);

int login();

int setPassiveMode();

int retrieve();

int closeConnection();

int endConnection();

#endif
