#ifndef CFG_H
#define CFG_H

typedef struct {
    int sn;
    int sx;
    int rd;
    int zc;
    int zl;
    int ec;
    int et;
    int sd;
    int pt;
    int cs;
    int bs;
    int vc;
    char lf[512];
    char td[512];
    int ll;
} Cfg;

extern Cfg gcfg;

void c_def(void);
int c_ld(const char *fn);
int c_sv(const char *fn);
void c_prt(void);

#endif
