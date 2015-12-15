#include <stdio.h>

#include "FTP.h"

int main(int argv, char* args[]){

	printf("\n[MAIN] Initializing application\n");


	/* Check if user typed right number of arguments */
	if (argv != 2){
        printf("[MAIN] Wrong number of arguments! Usage: app ftp://[<user>:<password>@]<host>/<url-path>\n - Use: ftp://speedtest.tele2.net/\n");
        return ERROR;
	}


	/* Parse the url and store the values in the ftp structure created in FTP.h */
	printf("[MAIN] Parsing the given URL\n");
	if(parseUrl(args[1]) == ERROR){
		printf("[MAIN] Invalid url format\n");
		exit(1);
	}else{
		printf("[MAIN] Valid url format\n");
	}


	/* Create Sockets */
	printf("[MAIN] Creating socket connection\n");
	if(openConnection() == ERROR) {
		printf("[MAIN] Error in opening connection\n");
		exit(1);
	}else{
		printf("[MAIN] Connection openned\n");
	}


	/* Log In */
	printf("[MAIN] Logging in \n");
	if(login() == ERROR){
		printf("[MAIN] Error in logging in\n");
		exit(1);
	}else{
		printf("[MAIN] Login sucssessfull\n");
	}


	/* Sets passive mode on */
	if(setPassive() == ERROR){
		printf("[MAIN] passive mode activation failed\n");
		exit(1);
	}else{
		printf("[MAIN] passive mode activated\n");
	}


	/* Close connection */
	printf("[MAIN] closing connection\n");
	if(closeConnection() == ERROR){
		printf("[MAIN] Error closing connection\n");
	}else{
		printf("[MAIN] Connection closed\n");
	}

	exit(0);
}
