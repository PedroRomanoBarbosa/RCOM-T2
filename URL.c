#include "URL.h"

char* pattern = "ftp://(([a-zA-Z0-9]+):([a-zA-Z0-9]+)@)?([\\.a-zA-Z0-9:]+)/([\\./a-zA-Z0-9]+)$";

int regexMatch(char* src, char* matches[]){
	int i;
	int res;
	int length;
	char result[BUFSIZ];
	char err_buf[BUFSIZ];
	regex_t* regex = (regex_t*)malloc(sizeof(regex_t));

	/* array of results */
	regmatch_t pmatch[N_MATCH];

	if( (res = regcomp(regex, pattern, REG_EXTENDED)) != 0) {
		regerror(res, regex, err_buf, BUFSIZ);
		printf("[URL] regcomp: %s\n", err_buf);
		return ERROR;
	}

	/* Execute regular expression */
	res = regexec(regex, src, N_MATCH, pmatch, REG_NOTBOL);
	
	/* Check if there was no match */
	if(res == REG_NOMATCH) {
		return ERROR;
	}

	int size = 0;
	/* Iterate over matches array and check if the user is logged in as anonymous */
	for (i = 2; i < N_MATCH; i++) {
		/* Get sub string */
		length = pmatch[i].rm_eo - pmatch[i].rm_so;
		memcpy(result, src + pmatch[i].rm_so, length);
		result[length] = 0;
		matches[i-2] = malloc(sizeof(char)*length+1);
		strcpy(matches[i-2],result);
		size++;
	}

	/* Free regular expression memory */
	regfree(regex);

	return size;
}