#ifndef _URL_H_
#define _URL_H_

#include <regex.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"


/**
* Gets the results parsed by an FTP pattern on a given string
*/
int regexMatch(char* src, char* matches[]);

#endif
