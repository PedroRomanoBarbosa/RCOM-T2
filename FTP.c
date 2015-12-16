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
					FTPUrl.username = "anonymous";
					FTPUrl.password = "123456";
				}else{
					FTPUrl.anon = FALSE;
					FTPUrl.username = match;
				}
				break;
			case PASSWORD:
				if(!FTPUrl.anon){
					FTPUrl.password = match;
				}
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

int printStruct(){
	printf("	[FTP] ## URL STRUCTURE ##\n");
	printf("	[FTP] username: %s\n", FTPUrl.username);
	printf("	[FTP] password: %s\n", FTPUrl.password);
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

int createSocket(char* host, int port, int* sock){
	struct	sockaddr_in server_addr;
	struct hostent *h;
	char* ip;

	/* Gets server ip address from DNS name */
	if ((h = gethostbyname(host)) == NULL) {  
		herror("	[FTP] gethostbyname");
		return ERROR;
	}
	ip = inet_ntoa(*((struct in_addr*)h->h_addr));

	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	/*open an TCP socket*/
	printf("	[FTP] Creating FTP socket to %s\n", ip);
	if ((*sock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		printf(" [FTP] Error opening socket\n");
    	return ERROR;
	}

	/*connect to the server*/
	if(connect(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
	    printf("	[FTP] Error connecting to socket\n");
		return ERROR;
	}

	return OK;
}

int openConnection(){

	int n;
	char response[MAX_RESP];
	int respNumber;

	if( createSocket(FTPUrl.host, FTP_PORT, &FTPSocket.sockfd) == ERROR){
		printf("	[FTP] Error creating connection socket\n");
	}

	/* Read the response from the server */
	if( (n = read(FTPSocket.sockfd, response, sizeof(response))) == ERROR){
		perror("	[FTP] Error readin return response from server\n");
		return ERROR;
	}
	if(sscanf(response,"%d", &respNumber) == EOF){
		printf("	[FTP] Error fetching number from response\n");
		return ERROR;
	}
	printf("	[FTP] < %d\n", respNumber);
	if(respNumber != CONN){
		printf("	[FTP] Invalid response from server\n");
		return ERROR;
	}

	return OK;
}

int login(){

	if(userCommand() == ERROR){
		return ERROR;
	}

	if(passwordCommand() == ERROR){
		return ERROR;
	}

	return OK;
}

int userCommand(){
	char userCmd[MAX_CMD];
	char response[MAX_RESP];
	int n;

	sprintf(userCmd,"USER %s\r\n", FTPUrl.username);
	printf("	[FTP] > USER %s\n", FTPUrl.username);

	n = sendCommand(userCmd, response);
	if(n == ERROR){
		printf("	[FTP] Error executing USER command\n");
		return ERROR;
	}else if(n != USER_VALID){
		printf("	[FTP] Invalid response from the server\n");
		return ERROR;
	}

	return OK;
}

int passwordCommand(){
	char passwordCmd[MAX_CMD];
	char response[MAX_RESP];
	int n;

	sprintf(passwordCmd,"PASS %s\r\n", FTPUrl.password);
	printf("	[FTP] > PASS %s\n", FTPUrl.password);

	n = sendCommand(passwordCmd, response);
	if(n == ERROR){
		printf("	[FTP] Error executing PASS command\n");
		return ERROR;
	}else if(n != PASS_VALID){
		printf("	[FTP] Invalid response from the server\n");
		return ERROR;
	}

	return OK;
}

int sendCommand(char* cmd, char resp[]){
	int respNumber;
	char response[MAX_RESP];
	int n;

	if( (n = write(FTPSocket.sockfd, cmd, strlen(cmd))) == ERROR){
		perror("	[FTP] Error writting command to server: ");
		printf("%s\n", cmd);
		return ERROR;
	}

	if( (n = read(FTPSocket.sockfd, response, sizeof(response))) == ERROR){
		perror("	[FTP] Error reading return response from server\n");
		return ERROR;
	}
	if(sscanf(response,"%d", &respNumber) == EOF){
		printf("	[FTP] Error fetching number from response\n");
		return ERROR;
	}
	printf("	[FTP] < %d\n", respNumber);

	memcpy(resp,response,strlen(response));

	return respNumber;
}

int setPassive(){
	int n;
	char* pasvCmd = "PASV\r\n";
	char response[MAX_RESP];
	int respAddr[IP_LENGTH];
	int respPort[PORT_LENGTH];
	char addr[MAX_IP_LENGTH];
	int port;

	n = sendCommand(pasvCmd, response);
	if(n == ERROR){
		printf("	[FTP] Error executing PASV command\n");
		return ERROR;
	}else if(n != PASV_VALID){
		printf("	[FTP] Invalid response from the server\n");
		return ERROR;
	}

	/* Retrieve ip address and port */
	if(sscanf(response,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &respAddr[0], &respAddr[1], &respAddr[2], &respAddr[3], &respPort[0], &respPort[1]) == EOF){
		printf("	[FTP] Error fetching ip and port number from response\n");
		return ERROR;
	}

	/* Build ip address and port */
	if(sprintf(addr, "%d.%d.%d.%d",respAddr[0], respAddr[1], respAddr[2], respAddr[3]) < 0){
		printf("	[FTP] Error creating full ip address\n");
		return ERROR;
	}
	port = respPort[0] * 256 + respPort[1];
	printf("	[FTP] passive mode ip address: %s:%d\n", addr, port);

	/* Create a data socket */
	createSocket(addr, port, &FTPSocket.datafd);
	if(FTPSocket.datafd < 0){
		printf("	[FTP] Error creating data socket\n");
		return ERROR;
	}

	return OK;
}

int getFileSize(){
	
	char sizeCmd[MAX_CMD];
	char response[MAX_RESP];
	int n;
	
	sprintf(sizeCmd,"SIZE %s\r\n", FTPUrl.filePath);
	printf("	[FTP] > SIZE %s\n", FTPUrl.filePath);
	
	n = sendCommand(sizeCmd, response);
	sscanf(response,"%d %lu", &n, &FTPUrl.fileSize); 
	if(n == PATH_INVALID){
		printf("	[FTP] Error checking file. File not found\n");
		return ERROR;
	}else if(n == SIZE_VALID){
		printf("	[FTP] Size: %lu\n",FTPUrl.fileSize);
	}else{
		printf("	[FTP] Error checking file\n");
		return ERROR;
	}
	
	return OK;
}

int transfer(){
	char retrieveCmd[MAX_CMD];
	char response[MAX_RESP];
	int n;
	unsigned long number = 0;
	
	FILE* fp = fopen(basename(FTPUrl.filePath),"w");
	if(!fp){
		printf("	[FTP] Error opening local file\n");
		return ERROR;
	}
	
	sprintf(retrieveCmd,"RETR %s\r\n", FTPUrl.filePath);
	printf("	[FTP] > RETR %s\n", FTPUrl.filePath);
	
	n = sendCommand(retrieveCmd, response);
	if(n == PATH_INVALID){
		printf("	[FTP] Error opening file. File not found\n");
		return ERROR;
	}else if(n == FILE_STATUS){
		printf("	[FTP] File found\n");
	}else{
		printf("	[FTP] Error getting file\n");
		return ERROR;
	}
	
	/* Get data packages */
	char package[MAX_PACK];
	int m;
	while( (m = read(FTPSocket.datafd, package, sizeof(package))) ){
		if(m == ERROR){
			perror("	[FTP] Error reading package from server\n");
			return ERROR;
		}else{
			number += m;
			if(fwrite(package, m, sizeof(char), fp) < 0){
				fclose(fp);
				printf("	[FTP] Error writting data to local file\n");
				return ERROR;
			}
		}
	}
	
	/* Verify file size */
	if(number != FTPUrl.fileSize){
		printf("	[FTP] Local file doesn't have the expected size\n");
		return ERROR;
	}
	
	return OK;
}

int endConnection(){
  return OK;
}

int download(){
  return OK;
}

int closeConnection(){

	if(close(FTPSocket.sockfd) == ERROR){
		perror("	[FTP] Error closing connection socket");
		return ERROR;
	}

	if(close(FTPSocket.datafd) == ERROR){
		perror("	[FTP] Error closing data socket");
		return ERROR;
	}
    
    return OK;
}
