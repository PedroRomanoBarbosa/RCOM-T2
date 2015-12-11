#include "FTP.h"

int FTPinitializeStruct(char* matches[], int size){
	int i;
	char* match;
	/* Iterate over matches array and check if the user is logged in as anonymous */
	for (i = 0; i < size; i++) {
		match = matches[i];
		/* Fill the url structure */
		switch(i){
			case USER_NAME:
				/* Check if the user is anonymous or not */
				if(strcmp(match,"") == 0){
					FTPurl.anon = TRUE;
				}else{
					FTPurl.anon = FALSE;
					FTPurl.username = match;
				}
				break;
			case PASSWORD:
				FTPurl.password = match;
				break;
			case HOST:
				FTPurl.host = match;
				break;
			case FILE_PATH:
				FTPurl.filePath = match;
				break;
		}
	}

	return OK;
}

int FTPSendRec(int sockfd, char* buf, char* response){
  return OK;
}

int FTPprintStruct(){
	printf("[FTP] ## URL STRUCTURE ##\n");
	if(FTPurl.anon){
		printf("[FTP] username: anonymous\n");
		printf("[FTP] password: 123456\n");
	}else{
		printf("[FTP] username: %s\n", FTPurl.username);
		printf("[FTP] password: %s\n", FTPurl.password);
	}
	printf("[FTP] host: %s\n", FTPurl.host);
	printf("[FTP] path: %s\n", FTPurl.filePath);

	return OK;
}


int FTPParseUrl(char* str){
	char* matches[N_MATCH];
	int size;

	if( (size = regexMatch(str, matches)) > 0){
		FTPinitializeStruct(matches,size);
		FTPprintStruct();
		return OK;
	}

	return ERROR;
}

int FTPOpen(){
  return OK;
}

int FTPLogin(){
  return OK;
}

int FTPSetPassiveMode(){
  return OK;
}

int FTPRetrieve(){
  return OK;
}

int FTPEnd(){
  return OK;
}

int FTPDownload(){
  return OK;
}

int FTPClose(){
  return OK;
}
