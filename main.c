#include <stdio.h>

#include "FTP.h"
#include "URL.h"

int initialize(char* url){
	return OK;
}

int main(int argv, char* args[]){

	/* Check if user typed right number of arguments */
	if (argv != 2){
        printf("[MAIN] Wrong number of arguments! Usage: app ftp://[<user>:<password>@]<host>/<url-path>\n");
        return ERROR;
	}

	if(FTPParseUrl(args[1]) == ERROR){
		printf("[MAIN] Invalid url format\n");
		exit(1);
	}else{
		printf("[MAIN] Valid url format\n");
	}

	if(initialize(args[1]) == OK){
		
	}

	exit(0);
}
