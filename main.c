#include <stdio.h>

int main(int argv, char* args[]){

  if (argc != 2){
        printf("[MAIN] Wrong number of arguments. Usage: app ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

  return 0;
}

int initialize(char* url){
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
