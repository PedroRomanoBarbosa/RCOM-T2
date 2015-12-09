#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#include "FTP.h"

int initialize(char* url){
	char* test = (char*) malloc(sizeof(char)*10);
	return OK;
}

int regex(char* src){
	int i = 0;
	int res;
	int len;
	char result[BUFSIZ];
	char err_buf[BUFSIZ];  

	/* The regular expression pattern (might be changed) */
	const char* pattern = "(([a-z0-9]+):([a-z0-9]+)@)*([\\.a-z0-9]+)/([/a-z0-9]+)$";
	regex_t* regex = (regex_t*)malloc(sizeof(regex_t));

	regmatch_t pmatch[10];

	if( (res = regcomp(regex, pattern, REG_EXTENDED)) != 0) {
		regerror(res, regex, err_buf, BUFSIZ);
		printf("regcomp: %s\n", err_buf);
		exit(res);
	}

	res = regexec(regex, src, 10, pmatch, REG_NOTBOL);
	//~ res = regexec(&preg, src, 10, pmatch, 0);
	//~ res = regexec(&preg, src, 10, pmatch, REG_NOTEOL);
	if(res == REG_NOMATCH) {
		printf("NO match\n");
		exit(0);
	}
	for (i = 0; pmatch[i].rm_so != -1; i++) {
		len = pmatch[i].rm_eo - pmatch[i].rm_so;
		memcpy(result, src + pmatch[i].rm_so, len);
		result[len] = 0;
		printf("num %d: '%s'\n", i, result);
	}
	regfree(regex);
	return 0;
}

int main(int argv, char* args[]){

	/* Check if user typed right number of arguments */
	if (argv != 2){
        printf("[MAIN] Wrong number of arguments! Usage: app ftp://[<user>:<password>@]<host>/<url-path>\n");
        return ERROR;
	}

	regex(args[1]);

	if(initialize(args[1]) == OK){
		return OK;
		printf("%s\n", args[1]);
	}

	return OK;
}
