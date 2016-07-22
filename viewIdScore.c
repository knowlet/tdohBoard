#include <stdio.h>
#include <stdlib.h>
#include "sha1.h"
#define PROBNUM 5

const static char* probName[PROBNUM] = {
    "Base64x16",
    "Crypto",
    "Loser'sFileReader",
    "findMyKey",
    "tdohLoginSystem"
};

void sha1(const char* input, char* hashascii)
{
    int i = 0;
    SHA1_CTX ctx;
    unsigned char hash[20];
    SHA1Init(&ctx);
    SHA1Update(&ctx, input, strlen(input));
    SHA1Final(hash, &ctx);
    for (i = 0; i < 20; ++i)
        sprintf(hashascii + i * 2, "%02X", hash[i]);
}

int main(int argc, char const *argv[])
{
    char id[32] = "";
    char hash[48] = "";
    char path[255] = "data/users/";
    char flags[10] = "";
    FILE *fp = NULL;
    printf("id: ");
    fflush(stdout);
    scanf("%25s", id);
    sha1(id, hash);
    strcat(path, hash);
    if ((fp=fopen(path, "r")) != NULL) {
        int i = 0, count = 0;
        fseek(fp, 40, SEEK_SET);
        fread(flags, sizeof(char), PROBNUM, fp);
        for (i = 0; i < PROBNUM; ++i) {
            printf("%-32s%s\n", probName[i], flags[i] ? "(pass" : "(not pass");
            count += flags[i];
        }
        if (count == PROBNUM) {
            puts("破台啦！");
        }
    }
    else {
        perror("FILE IO ERROR");
        exit(1);
    }
    return 0;
}