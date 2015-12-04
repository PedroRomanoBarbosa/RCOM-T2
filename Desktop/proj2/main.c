#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "FTP.h"

void printStructInfo();

int main(int argc, char* argv[])
{
    if (argc != 2){
        printf(" > [MAIN] Wrong arguments. Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        return ERROR;
    }

	//Initialize the struct
    if (initializeStruct(argv[1]) != OK){
        printf(" > [MAIN] Error initializing FTPConnection struct\n");
        return ERROR;
    } else {
		printStructInfo();
	}

	//Set up the sockets and open connection to the FTP server
	printf(" > [MAIN] Opening Connection...\n");
    if(FTPOpenConnection() != OK){
        printf(" > [MAIN] Error establishing connecting.\n");
        return ERROR;
    } else {
		printf(" > [MAIN] Connection established\n\n");
	}

	//Login
	printf(" > [MAIN] Logging in...\n");
	if(FTPLogin() != OK){
		printf(" > [MAIN] Error logging in\n");
		return ERROR;
	} else {
		printf(" > [MAIN] Logged in sucessfully\n\n");
	}
	
	//Set passive mode
	printf(" > [MAIN] Setting passive mode...\n");
	if(FTPSetPassiveMode() != OK){
		printf(" > [MAIN] Error setting passive mode\n");
		return ERROR;
	} else {
		printf(" > [MAIN] Passive mode set sucessfully\n\n");
	}
	
	//Retrieve the file
	printf(" > [MAIN] Retrieving file...\n");
	if(FTPRetrieve() != OK){
		printf(" > [MAIN] Error retrieving file\n");
		return ERROR;
	} else {
		printf(" > [MAIN] File retrieved sucessfully\n\n");
	}
	
	//Download the file
	printf(" > [MAIN] Downloading file...\n");
	if(FTPFileDownload() != OK){
		printf(" > [MAIN] Error downloading file\n");
		return ERROR;
	} else {
		printf(" > [MAIN] File downloaded sucessfully\n\n");
	}
	
	//Close the FTP server connection and sockets
	printf(" > [MAIN] Closing connection...\n");
    if (FTPEndConnection() != OK){
        printf(" > [MAIN] Error ending connection.\n");
        return ERROR;
    } else {
		printf(" > [MAIN] Connection ended sucessully\n\n");
	}

    return OK;
}

void printStructInfo(){
	printf("\n > [MAIN] FTPConnection Struct Initialized\n\n");
	printf(" ===== FTPConnection ===== \n");
	printf(" > [CONN] User: %s\n",conn.user);
	printf(" > [CONN] Pass: %s\n",conn.pass);
	printf(" > [CONN] Host: %s\n",conn.hostname);
	printf(" > [CONN] Path: %s\n",conn.path);
	printf(" > [CONN] Filename: %s\n",conn.filename);
	printf(" ===== FTPConnection ===== \n\n");
}

int initializeStruct(char* url){
    char* user = (char*) malloc(MAX_USER_SIZE*sizeof(char));
    char* pass = (char*) malloc(MAX_PASS_SIZE*sizeof(char));
    char* host = (char*) malloc(MAX_HOST_SIZE*sizeof(char));
    char* path = (char*) malloc(MAX_PATH_SIZE*sizeof(char));

    if(sscanf(url, "ftp://[%[^:]:%[^@]@]%[^/]/%s\n", user, pass, host, path) == 4){
        conn.user = user;
        conn.pass = pass;
        conn.hostname = host;
        conn.path = path;
        conn.filename = basename(path);

        return OK;   
    } else if(sscanf(url, "ftp://%[^/]/%s\n", host, path) == 2) {
        strncpy(user,"anonymous",strlen("anonymous")+1);
        strncpy(pass,"anonymous@email.com",strlen("anonymous@email.com")+1);
        
        conn.user = user;
        conn.pass = pass;
        conn.hostname = host;
        conn.path = path;
        conn.filename = basename(path);

        return OK;
    } else {
        printf(" > [MAIN] Invalid URL. Usage: dl ftp://[<user>:<password>@]<host>/<url-path>\n");
	}

    return ERROR;
}
