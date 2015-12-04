#include "FTP.h"

int FTPSendRec(int sockfd, char* buf, char* response){
    if(socketWrite(sockfd, buf, strlen(buf)) != OK){
        printf(" > [FTP] Error writing to Control Socket\n");
        return ERROR;
	}

	//usleep(100000);

    char FTPResponse[MAX_CTRL_LENGTH];

    if(socketRead(sockfd, FTPResponse, sizeof(FTPResponse)) != OK){
        printf(" > [FTP] Error reading from Control Socket\n");
        return ERROR;
	}
	
    response[0] = FTPResponse[0];
    response[1] = FTPResponse[1];
    response[2] = FTPResponse[2];
    
    return OK;
}

int FTPOpenConnection(){
	int sockfd = socketConnection(conn.hostname, FTP_PORT);
    if (sockfd < 0) {
    	printf(" > [FTP] Error connecting socket\n");
        return ERROR;
    } else {
		printf(" > [FTP] Control Socket opened sucessfully\n");
	}

    conn.ctrlSocket = sockfd;
    conn.dataSocket = 0;

    //usleep(500000);

    char buf[MAX_CTRL_LENGTH];
    if(socketRead(conn.ctrlSocket, buf, sizeof(buf)) != 0){
        printf(" > [FTP] Error reading the FTP greeting banner\n");
        return ERROR;
    }

    return OK;
}

int FTPLogin(){
    char buf[MAX_CTRL_LENGTH];
    char response[FTP_RESP_LENGTH];

    sprintf(buf, "USER %s\r\n", conn.user);
    if(FTPSendRec(conn.ctrlSocket, buf, response) != 0)
    {
    	printf("Error sending USER.\n");
        return ERROR;
    }
    
    //usleep(500000);
    
    sprintf(buf, "PASS %s\r\n", conn.pass);
    if(FTPSendRec(conn.ctrlSocket, buf, response) != 0){
		printf(" > [FTP] Error sending PASS\n");
        return ERROR;
	}

    if(response[0] == '2' && response[1] == '3' && response[2] == '0'){
        return OK;
	} else {
        printf(" > [FTP] Error logging in. Invalid USER or PASS\n");
        return ERROR;
    }
}

int FTPSetPassiveMode(){
	char buf[MAX_CTRL_LENGTH] = "PASV\r\n";

	if(socketWrite(conn.ctrlSocket, buf, strlen(buf)) != 0){
        printf(" > [MAIN] Error writing PASV command\n");
        return ERROR;
    }
       
	//usleep(100000);

    if(socketRead(conn.ctrlSocket, buf, sizeof(buf)) != OK){
        printf(" > [FTP] Error reading from socket while setting PASV mode\n");
        return ERROR;
	}

    int ip[IP_PASV_LENGTH];
    int port[PORT_PASV_LENGTH];

    if(sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]) < 0){
    	printf(" > [FTP] Error retrieving PASV Port|Ip information\n");
    	return ERROR;
    }

    char serverIp[IP_LENGTH];
    if(sprintf(serverIp, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]) < 0){
    	printf(" > [FTP] Error building Server IP\n");
    	return ERROR;
    }

    int serverPort = port[0] * 256 + port[1];

    int datasockfd = socketConnection(serverIp, serverPort);
    if (datasockfd < 0) {
        printf(" > [FTP] Error opening Data Socket\n");
        return ERROR;
    }

    conn.dataSocket = datasockfd;

    printf(" > [FTP] Data Socket opened sucessfully\n");
    //usleep(500000); 
    return OK;
}

int FTPRetrieve(){
    char buf[MAX_CTRL_LENGTH];
    char response[FTP_RESP_LENGTH];
    
    sprintf(buf, "RETR %s\r\n", conn.path);
    if(FTPSendRec(conn.ctrlSocket, buf, response) != OK){
        printf(" > [FTP] Error sending RETR command\n");
        return ERROR;
    }

    if(response[0] == '5' || response[0] == '4'){
        printf(" > [FTP] Error retrieving file\n");
        printf(" > Server Response: %s\n",response);
        return ERROR;
    }

    return OK;	
}

int FTPEndConnection(){
	char* buf = "QUIT\r\n";
    char response[FTP_RESP_LENGTH];

	if(FTPSendRec(conn.ctrlSocket, buf, response) != OK){
    	printf(" > [FTP] Error sending QUIT command\n");
    	return ERROR;
	}

    return FTPCloseConnection();	
}

int FTPFileDownload(){
	FILE* fp = fopen(conn.filename, "w");
    if (!fp){
        printf(" > [FTP] Error opening file\n");
        return ERROR;
    }

    char buf[MAX_DATA_LENGTH];
    int nread;
    while ((nread = read(conn.dataSocket, buf, MAX_DATA_LENGTH))) {
        if (nread < 0){
            printf(" > [FTP] Error reading from Data Socket\n");
            return ERROR;
        }
		
		printf(" > [FTP] Received a file chunk -- %d bytes\n",nread);
		
        int nwrote = fwrite(buf, nread, sizeof(char), fp);
        if (nwrote < 0){
            printf(" > [FTP] Error writing to file\n");
            return ERROR;
        }
    }

    fclose(fp);

    return OK;
}

int FTPCloseConnection(){
	close(conn.ctrlSocket);
	close(conn.dataSocket);
	
    free(conn.user);
    free(conn.pass);
    free(conn.hostname);
    free(conn.path);
    
    return OK;
}
















