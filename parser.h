#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


/**
* Gets the results parsed by an FTP pattern on a given string
*/
int regexMatch(char* src, char* matches[]);

#endif
