#include "tdohBoard.h"
#include "sha1.h"

const static char* flags[] = {
    "TDOH{Y0U_AR3_R3A11Y_A_B0T}",
    "TDOH{CAESAR_IS_PROUD_OF_YOU}",
    "TDOH{Y0U_AR3_7H3_\\/\\/1NN3R}",
    "TDOH{/*_N0THING_C0NFUSES_U!!_*/}",
    "TDOH{Y0U_B3A7_7H3_R/\\ND0M}"
};
const static char* probName[PROBNUM] = {
    "Base64x16",
    "Crypto",
    "Loser'sFileReader",
    "findMyKey",
    "tdohLoginSystem"
};
static char id[32] = "";
static char inputFlag[48] = "";
static char ansFlags[10] = {};
static FILE *fp = NULL;
static USER *user = NULL;

int scan(FILE *fp, char *buffer, size_t bufsiz)
{
    char format[16];
    snprintf(format, sizeof(format), "%%%zus", bufsiz - 1);
    return fscanf(fp, format, buffer);
}

int noBlackBlackInput()
{
    char input[4] = "";
    scan(stdin, input, 2);
    fflush(stdin);
    if (!isdigit(*input)) {
        puts("Input Error! Not a number");
        fflush(stdout);
        return 0;
    }
    return *input ^ 0x30;
}

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

int checkNotPrintable(char* str, int len)
{
    int i = 0;
    for (i = 0; i < len; ++i) {
        if (i > 0 && !str[i]) return 0;
        if (!isprint(str[i])) return 1;
    }
    return 0;
}

void printMenu()
{
    puts("\t                                         ");
    puts("\t ---- Welcome to TDoHacker wargame! ---- ");
    puts("\t|                                       |");
    puts("\t| [1] Input Key                         |");
    puts("\t| [2] Bye~                              |");
    puts("\t|                                       |");
    puts("\t --------------------------------------- ");
    printf("What are you going to do? ");
    fflush(stdout);
}

void printKeyMenu()
{
    int i = 0;
    puts("");
    puts("\t ---- Welcome to TDoHacker wargame! ---- ");
    puts("\t|                                       |");
    for (i = 0; i < PROBNUM; ++i)
        printf("\t| [%d] %-25s%8s |\n", i + 1, probName[i], user->flags[i] ? "(pass)" : "");
    puts("\t|                                       |");
    puts("\t --------------------------------------- ");
    fflush(stdout);
}

void inputKey()
{
    unsigned ch = 0;
    char buff[255] = "";
    puts("");
    printf("Hello %s, which flag are you going to input?\n", id);
    printKeyMenu();
    ch = noBlackBlackInput();
    if (!ch || ch > PROBNUM) return;
    if (user->flags[ch-1]) {
        puts("You had passed this already.");
        fflush(stdout);
        return;
    }
    printf("Please input your flag (format: TDOH{printable_ascii}): ");
    fflush(stdout);
    scan(stdin, buff, 40);
    sscanf(buff, "%s", inputFlag);
    if (checkNotPrintable(inputFlag, 40)) {
        puts("I don't think this is a key...");
        puts("You shall not pass!");
        fflush(stdout);
        exit(1);
    }
    if (!strncmp(inputFlag, flags[ch-1], strlen(flags[ch-1]))) {
        puts("Correct! you're good at it!");
        fflush(stdout);
        user->flags[ch-1] = 1;
        fseek(fp, 40, SEEK_SET);
        fwrite(user->flags, sizeof(char), PROBNUM, fp);
    }
    else {
        puts("Sorry! you shall not pass OwQ");
        fflush(stdout);
    }
}

void bye()
{
    puts("歡迎來 TDoH 的攤位晃晃喔～");
    puts("Bye～");
    fflush(stdout);
    exit(0);
}

int checkExist(char* hash)
{
    FILE *file;
    struct stat st;
    int result = stat(hash, &st);
    return result == 0;
}

void magic()
{
    exit(0);
}

static void (*menuFunc[])(void) = {magic, inputKey, bye};
int main(int argc, char const *argv[])
{
    user = calloc(1, sizeof(USER));
    unsigned int ch = -1;
    int isExist = 0;
    char hash[20] = "";
    char passwd[48] = "";
    char passHash[40] = "";
    char path[255] = "data/users/";

    printf("Please input id (max: 25): ");
    fflush(stdout);
    scan(stdin, id, 25);

    if (checkNotPrintable(id, 25)) exit(1);
    sha1(id, hash);

    strcat(path, hash);
    isExist = checkExist(path);
    if (isExist) 
        printf("Input passwd to login: ");
    else {
        printf("Input passwd to create (max: 40): ");
    }

    scan(stdin, passwd, 40);
    sha1(passwd, passHash);
    if (checkNotPrintable(id, 40)) exit(1);
    if (!isExist) {
        if ((fp=fopen(path, "wb")) != NULL) {
            puts("Account Created!");
            strncpy(user->passwd, passHash, 40);
            fwrite(user, sizeof(USER), 1, fp);
        }
        else {
            perror("FILE IO ERROR");
            exit(1);
        }
        fclose(fp);
    }

    if ((fp=fopen(path, "rb+")) != NULL) {
        puts("Trying to login...");
        fread(user, sizeof(USER), 1, fp);
        if (strncmp(user->passwd, passHash, 40)) {
            puts("Wrong password!");
            exit(1);
        }
    }
    else {
        perror("FILE IO ERROR");
        exit(1);
    }

    while (ch) {
        printMenu();
        ch = noBlackBlackInput();
        fflush(stdout);
        if (ch > 3) {
            printf("%s\n", "Func not found");
            fflush(stdout);
        }
        else {
            (*menuFunc[ch])();
        }
    }
    fclose(fp);
    return 0;
}