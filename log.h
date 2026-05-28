#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>

typedef enum {
    DBG = 0,
    INF = 1,
    WRN = 2,
    ERR = 3,
    FTL = 4
} LLvl;

typedef struct {
    FILE *f;
    LLvl l;
    int ts;
    int c;
} Log;

extern Log glog;

void l_ini(const char *fn, LLvl lv, int col);
void l_cls(void);
void l_msg(LLvl lv, const char *fmt, ...);
void l_dbg(const char *fmt, ...);
void l_inf(const char *fmt, ...);
void l_wrn(const char *fmt, ...);
void l_err(const char *fmt, ...);
void l_ftl(const char *fmt, ...);

#define LFILE "odium.log"

#endif
