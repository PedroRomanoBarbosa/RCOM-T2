#include "sockets.h"

int socketConnection(char* hostname, int port){
	int sockfd;
	struct sockaddr_in serverAddr;
	
	struct hostent *server = gethostbyname(hostname);
	if(server == NULL){
		printf(" > [SOCKETS] Cannot find HOST\n");
		return ERROR;
	}
	
	memset(&serverAddr, 0 , sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	bcopy((char*)server->h_addr, (char*)&serverAddr.sin_addr.s_addr, server->h_length);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0){
		printf(" > [SOCKETS] Error opening socket\n");
		return ERROR;
	}
	
	if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf(" > [SOCKETS] Error connecting socket\n");
		return ERROR;
    }
	
	return sockfd;
}

int socketRead(int sockfd, char* buf, int length){
	int nread = read(sockfd, buf, length);
	if(nread <= 0){
		printf(" > [SOCKETS] Error reading from socket\n");
		return ERROR;
	}
	return OK;
}

int socketWrite(int sockfd, char* buf, int length){
	int nwrote = write(sockfd, buf, length);
	if(nwrote < 0){
		printf(" > [SOCKETS] Error writing to socket\n");
		return ERROR;
	}
	return OK;
}
