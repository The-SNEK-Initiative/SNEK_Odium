#include "log.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

Log glog = {NULL, INF, 1, 1};

void l_ini(const char *fn, LLvl lv, int col) {
    glog.l = lv;
    glog.c = col;
    glog.ts = 1;
    
    if (fn) {
        glog.f = fopen(fn, "a");
        if (!glog.f) {
            fprintf(stderr, "Failed to open log file: %s\n", fn);
            glog.f = stderr;
        }
    } else {
        glog.f = stdout;
    }
}

void l_cls(void) {
    if (glog.f && glog.f != stdout && glog.f != stderr) {
        fclose(glog.f);
        glog.f = NULL;
    }
}

const char* ls_lvl(LLvl lv) {
    switch (lv) {
        case DBG: return "DBG";
        case INF: return "INF";
        case WRN: return "WRN";
        case ERR: return "ERR";
        case FTL: return "FTL";
        default: return "UNK";
    }
}

const char* lc_cd(LLvl lv) {
    if (!glog.c) return "";
    switch (lv) {
        case DBG: return "\033[36m";
        case INF: return "\033[32m";
        case WRN: return "\033[33m";
        case ERR: return "\033[31m";
        case FTL: return "\033[35m";
        default: return "";
    }
}

const char* lc_rs(void) {
    return glog.c ? "\033[0m" : "";
}

void l_msg(LLvl lv, const char *fmt, ...) {
    if (lv < glog.l || !glog.f) return;
    
    va_list args;
    va_start(args, fmt);
    
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    
    fprintf(glog.f, "%s%s [%s]%s ",
            lc_cd(lv), ts, ls_lvl(lv), lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    
    va_end(args);
}

void l_dbg(const char *fmt, ...) {
    if (DBG < glog.l) return;
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    fprintf(glog.f, "%s%s [DBG]%s ", lc_cd(DBG), ts, lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    va_end(args);
}

void l_inf(const char *fmt, ...) {
    if (INF < glog.l) return;
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    fprintf(glog.f, "%s%s [INF]%s ", lc_cd(INF), ts, lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    va_end(args);
}

void l_wrn(const char *fmt, ...) {
    if (WRN < glog.l) return;
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    fprintf(glog.f, "%s%s [WRN]%s ", lc_cd(WRN), ts, lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    va_end(args);
}

void l_err(const char *fmt, ...) {
    if (ERR < glog.l) return;
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    fprintf(glog.f, "%s%s [ERR]%s ", lc_cd(ERR), ts, lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    va_end(args);
}

void l_ftl(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    time_t now = time(NULL);
    struct tm *ti = localtime(&now);
    char ts[20];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", ti);
    fprintf(glog.f, "%s%s [FTL]%s ", lc_cd(FTL), ts, lc_rs());
    vfprintf(glog.f, fmt, args);
    fprintf(glog.f, "\n");
    fflush(glog.f);
    va_end(args);
}
