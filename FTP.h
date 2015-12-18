#ifndef _FTP_H_
#define _FTP_H_

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libgen.h>

#include "parser.h"
#include "macros.h"

#define USER_NAME 0
#define PASSWORD 1
#define HOST 2
#define FILE_PATH 3

#define FTP_PORT 21
#define MAX_RESP 256
#define MAX_CMD 256
#define IP_LENGTH 4
#define PORT_LENGTH 2
#define MAX_IP_LENGTH 16
#define MAX_PACK 256

#define CONN 220
#define USER_VALID 331
#define PASS_VALID 230
#define PASV_VALID 227
#define PATH_INVALID 550
#define SIZE_VALID 213
#define FILE_STATUS 150
#define CONN_END 226

struct url{
	int anon;
	char* username;
	char* password;
	char* host;
	char* filePath;
	unsigned long fileSize;
} FTPUrl;

struct socket{
	int	sockfd;
	int datafd;
} FTPSocket;

int initializeStruct(char* matches[], int size);

int printStruct();

int createSocket(char* host, int port, int* socket);

int openConnection();

int parseUrl(char* str);

int login();

int userCommand();

int passwordCommand();

int sendCommand(char* cmd, char resp[]);

int setPassive();

int getFileSize();

int transfer();

int closeConnection();

#endif
