#ifndef CLI_H
#define CLI_H

typedef enum {
    INT = 0,
    SHD = 1,
    RES = 2,
    VER = 3,
    LST = 4,
    BSH = 5,
    BRS = 6,
    GEN = 7,
    NFIO = 8,
    VRN = 9,
    HLP = 10,
    UNK = -1
} CCmd;

typedef struct {
    CCmd c;
    char in[512];
    char od[512];
    char cf[512];
    int k;
    int v;
    int t;
    int vf;
    int cmp;
    int et;
} CLIArg;

CLIArg* c_prs(int argc, char **argv);
void c_fr(CLIArg *args);
void c_hlp(void);
void c_vrn(void);

#endif
