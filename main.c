#include <stdio.h>

#include "FTP.h"

int main(int argv, char* args[]){

	printf("\n[MAIN] Initializing application\n");

	/* Check if user typed right number of arguments */
	if (argv != 2){
        printf("[MAIN] Wrong number of arguments! Usage: app ftp://[<user>:<password>@]<host>/<url-path>\n");
        return ERROR;
	}

	/* Parse the url and store the values in the ftp structure created in FTP.h */
	if(parseUrl(args[1]) == ERROR){
		printf("[MAIN] Invalid url format\n");
		exit(1);
	}else{
		printf("[MAIN] Valid url format\n");
	}

	//CREATE SOCKETS
	if (openConnection() == ERROR) {
		printf("[MAIN] Error in opening connection\n");
		exit(1);
	}else{
		printf("[MAIN] Connection openned\n");
	}
	//CONNECT

	//USER 

	//PASS

	//PASV

	//RETRIEVE

	//QUIT

	exit(0);
}
