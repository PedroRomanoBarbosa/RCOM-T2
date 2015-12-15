#include "FTP.h"

int initializeStruct(char* matches[], int size){
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
					FTPUrl.anon = TRUE;
				}else{
					FTPUrl.anon = FALSE;
					FTPUrl.username = match;
				}
				break;
			case PASSWORD:
				FTPUrl.password = match;
				break;
			case HOST:
				FTPUrl.host = match;
				break;
			case FILE_PATH:
				FTPUrl.filePath = match;
				break;
		}
	}

	return OK;
}

int sendRec(int sockfd, char* buf, char* response){
  return OK;
}

int printStruct(){
	printf("	[FTP] ## URL STRUCTURE ##\n");
	if(FTPUrl.anon){
		printf("	[FTP] username: anonymous\n");
		printf("	[FTP] password: 123456\n");
	}else{
		printf("	[FTP] username: %s\n", FTPUrl.username);
		printf("	[FTP] password: %s\n", FTPUrl.password);
	}
	printf("	[FTP] host: %s\n", FTPUrl.host);
	printf("	[FTP] path: %s\n", FTPUrl.filePath);

	return OK;
}


int parseUrl(char* str){
	char* matches[N_MATCH];
	int size;

	if( (size = regexMatch(str, matches)) > 0){
		initializeStruct(matches,size);
		printStruct();
		return OK;
	}

	return ERROR;
}

int openConnection(){

	int	sockfd;
	struct	sockaddr_in server_addr;
	struct hostent *h;
	char* ip;

	/* Gets server ip address from DNS name */
	if ((h = gethostbyname(FTPUrl.host)) == NULL) {  
		herror("	[FTP] gethostbyname");
		return ERROR;
	}
	ip = inet_ntoa(*((struct in_addr*)h->h_addr));

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(FTP_PORT);

	/*open an TCP socket*/
	printf("	[FTP] Creating FTP socket to %s\n", ip);
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		printf(" [FTP] Error opening socket\n");
    	return ERROR;
	}

	/*connect to the server*/
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
	    printf("	[FTP] Error connecting to socket\n");
		return ERROR;
	}

	return OK;
}

int login(){
  return OK;
}

int setPassiveMode(){
  return OK;
}

int retrieve(){
  return OK;
}

int endConnection(){
  return OK;
}

int download(){
  return OK;
}

int closeConnection(){
  return OK;
}
