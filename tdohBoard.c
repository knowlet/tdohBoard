#include "tdohBoard.h"
#include "sha1.h"

static char id[32] = "";
static char inputFlag[48] = "";
static FILE *fp = NULL;
static PROB *prob = NULL;
static USER *user = NULL;

int scan(FILE *fp, char *buffer, size_t bufsiz)
{
    char format[16];
    snprintf(format, sizeof(format), "%%%zus", bufsiz - 1);
    return fscanf(fp, format, buffer);
}

int put(const char* str)
{
    int ret = put(str);
    fflush(stdout);
    return ret;
}

int noBlackBlackInput()
{
    char input[4] = "";
    scan(stdin, input, 2);
    if (!isdigit(*input)) {
        puts("Input Error! Not a number");
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
    puts("\t| [2] Read Code                         |");
    puts("\t| [3] Bye~                              |");
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
        printf("\t| [%d] %-25s%8s |\n", i + 1, prob[i].name, user->flags[i] ? "(pass)" : "");
    puts("\t|                                       |");
    puts("\t --------------------------------------- ");
    fflush(stdout);
}

int checkAllPass()
{
    int i = 0;
    for (i = 0; i < PROBNUM; ++i)
        if (!user->flags[i]) return 0;
    return 1;
}

void inputKey()
{
    unsigned ch = 0;
    char buff[255] = "";
    if (checkAllPass()) {
        puts("您已經破台惹！");
        return;
    }
    puts("");
    printf("Hello %s, which flag are you going to input?\n", id);
    printKeyMenu();
    ch = noBlackBlackInput();
    if (!ch || ch > PROBNUM) return;
    if (user->flags[ch-1]) {
        puts("You had passed this already.");
        return;
    }
    printf("Please input your flag (format: TDOH{printable_ascii}): ");
    fflush(stdout);
    scan(stdin, buff, 40);
    sscanf(buff, "%s", inputFlag);
    if (checkNotPrintable(inputFlag, 40)) {
        puts("I don't think this is a key...");
        puts("You shall not pass!");
        exit(1);
    }
    if (!strcmp(inputFlag, prob[ch-1].flag)) {
        puts("Correct! you're good at it!");
        user->flags[ch-1] = 1;
        fseek(fp, 40, SEEK_SET);
        fwrite(user->flags, sizeof(char), PROBNUM, fp);
        if (checkAllPass()) {
            puts("恭喜破台！");
            puts("請記得去 TDOH 攤位兌換精美小禮物喔！");
            exit(0);
        }
    }
    else {
        puts("Sorry! you shall not pass OwQ");
    }
}

void readCode()
{
    unsigned ch = 0;
    char path[255] = "data/code/";
    printKeyMenu();
    printf("Which code do you want to read? ");
    ch = noBlackBlackInput();
    if (!ch || ch > PROBNUM) return;
    path[strlen(path)] = '0' + ch;
    strcat(path, "/main");
    puts("");
    if ((fp=fopen(path, "r")) != NULL) {
        char c = EOF;
        do {
            putchar((c=fgetc(fp)));
        } while (c != EOF);
        fclose(fp);
    }
    else {
        puts("No source code for U ^_^");
    }
    return;
    
}

void bye()
{
    puts("歡迎來 TDoH 的攤位晃晃喔～");
    puts("Bye～");
    exit(0);
}

int checkExist(char* hash)
{
    struct stat st;
    int result = stat(hash, &st);
    return result == 0;
}

void magic()
{
    exit(0);
}

static void (*menuFunc[])(void) = {magic, inputKey, readCode, bye};
int main(int argc, char const *argv[])
{
    const unsigned menuNum = sizeof(menuFunc) / sizeof(void*);
    prob = calloc(PROBNUM, sizeof(PROB));
    user = calloc(1, sizeof(USER));
    unsigned int ch = -1;
    int isExist = 0;
    char hash[20] = "";
    char passwd[48] = "";
    char passHash[40] = "";
    char path[255] = "data/users/";
    
    if ((fp=fopen("flag.dat", "rb")) != NULL) {
        fread(prob, sizeof(PROB), PROBNUM, fp);
        fclose(fp);
    }
    else {
        puts("Init false exiting...");
        exit(1);
    }
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
    fflush(stdout);
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
        if (strncmp(user->passwd, passHash, strlen(passHash))) {
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
        if (ch > menuNum) {
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
