#ifndef MAN_H
#define MAN_H

#include "has.h"
#include <time.h>

typedef struct {
    char fn[512];
    unsigned long sz;
    char md[33];
    char sh[65];
    int sc;
    int rd;
    int zc;
    int et;
    int k;
    time_t ct;
    time_t mt;
    char cr[128];
    char ver[16];
    int st;
} MEntry;

typedef struct {
    MEntry *e;
    int c;
    int cap;
    char db[512];
} ManDB;

ManDB* m_new(const char *db);
void m_del(ManDB *db);
int m_add(ManDB *db, MEntry *en);
int m_fnd(ManDB *db, const char *fn, MEntry *en);
int m_ld(ManDB *db);
int m_sv(ManDB *db);
int m_lst(ManDB *db);
void m_prt(MEntry *en);

#endif
