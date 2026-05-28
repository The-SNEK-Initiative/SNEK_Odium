#ifndef PRG_H
#define PRG_H

#include <stdio.h>
#include <time.h>

typedef struct {
    long tot;
    long cur;
    time_t st;
    char fn[512];
    int sh;
} Prog;

Prog* p_new(long tot, int sh);
void p_del(Prog *pb);
void p_upd(Prog *pb, long cur);
void p_sfn(Prog *pb, const char *fn);
void p_dsp(Prog *pb);
void p_cmpl(Prog *pb);
double p_pct(Prog *pb);
time_t p_el(Prog *pb);
time_t p_eta(Prog *pb);

#endif
