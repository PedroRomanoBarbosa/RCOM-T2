#ifndef _FTP_H_
#define _FTP_H_

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "parser.h"
#include "macros.h"

#define USER_NAME 0
#define PASSWORD 1
#define HOST 2
#define FILE_PATH 3

#define FTP_PORT 21
#define MAX_RESP 255
#define MAX_CMD 255
#define IP_LENGTH 4
#define PORT_LENGTH 2
#define MAX_IP_LENGTH 16

#define CONN 220
#define USER_VALID 331
#define PASS_VALID 230
#define PASV_VALID 227

struct url{
	int anon;
	char* username;
	char* password;
	char* host;
	char* filePath;
	int	sockfd;
} FTPUrl;

struct socket{
	int	sockfd;
} FTPSocket;

int initializeStruct(char* matches[], int size);

int printStruct();

int sendRec(int sockfd, char* buf, char* response);

int openConnection();

int parseUrl(char* str);

int login();

int userCommand();

int passwordCommand();

int sendCommand(char* cmd, char resp[]);

int setPassive();

int retrieve();

int closeConnection();

int endConnection();

#endif
