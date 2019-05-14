#include <stdio.h>
#include <stdlib.h>

typedef struct _prob
{
    char name[64];
    char flag[64];
} PROB;

int main(int argc, char const *argv[])
{
    PROB* prob = calloc(1, sizeof(PROB));
    FILE* fp = NULL;
    int op = 0;
    printf("1 write flag, 2 read flags: ");
    scanf("%d", &op);
    getchar();
    if (op == 1) {
        fp = fopen("flag.dat", "wb");
        while (gets(prob->name) && gets(prob->flag))
            fwrite(prob, sizeof(PROB), 1, fp);
    }
    else if (op == 2) {
        fp = fopen("flag.dat", "rb");
        while (!feof(fp)) {
            fread(prob, sizeof(PROB), 1, fp);
            puts(prob->name);
            puts(prob->flag);
        }
    }
    fclose(fp);
    free(prob);
    return 0;
}
