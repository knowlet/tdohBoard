#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define PROBNUM 5

typedef struct _user
{
    char passwd[40];
    unsigned char flags[PROBNUM];
} USER;