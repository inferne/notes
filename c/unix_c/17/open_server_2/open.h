#include "apue.h"
#include <error.h>

#define CL_OPEN "open" /* client's request for server */
#define CS_OPEN "/home/sar/opend" /* server's well-known name */

int csopen(char *, int);