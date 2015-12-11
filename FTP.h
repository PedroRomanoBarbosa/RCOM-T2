#ifndef _FTP_H_
#define _FTP_H_

#include "URL.h"
#include "Macros.h"

struct url{
	int anon;
	char* username;
	char* password;
	char* host;
	char* filePath;
} FTPurl;

int FTPinitializeStruct(char* matches[], int size);

int FTPprintStruct();

int FTPSendRec(int sockfd, char* buf, char* response);

int FTPOpen();

int FTPParseUrl(char* str);

int FTPLogin();

int FTPSetPassiveMode();

int FTPRetrieve();

int FTPCloseConnection();

#endif
