#include "prg.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

Prog* p_new(long tot, int sh) {
    Prog *pb = malloc(sizeof(Prog));
    if (!pb) return NULL;
    
    pb->tot = tot;
    pb->cur = 0;
    pb->st = time(NULL);
    pb->sh = sh;
    memset(pb->fn, 0, sizeof(pb->fn));
    
    return pb;
}

void p_del(Prog *pb) {
    if (pb) free(pb);
}

void p_upd(Prog *pb, long cur) {
    if (pb) {
        pb->cur = cur;
        if (pb->cur > pb->tot) pb->cur = pb->tot;
    }
}

void p_sfn(Prog *pb, const char *fn) {
    if (pb) {
        strncpy(pb->fn, fn, sizeof(pb->fn) - 1);
        pb->fn[sizeof(pb->fn) - 1] = '\0';
    }
}

double p_pct(Prog *pb) {
    if (!pb || pb->tot <= 0) return 0.0;
    return (100.0 * pb->cur) / pb->tot;
}

time_t p_el(Prog *pb) {
    if (!pb) return 0;
    return time(NULL) - pb->st;
}

time_t p_eta(Prog *pb) {
    if (!pb || pb->cur == 0 || pb->tot <= 0) return 0;
    time_t el = p_el(pb);
    double rt = (double)pb->cur / el;
    return (time_t)((pb->tot - pb->cur) / rt);
}

void p_dsp(Prog *pb) {
    if (!pb) return;
    
    double pct = p_pct(pb);
    time_t el = p_el(pb);
    time_t eta = p_eta(pb);
    
    int bw = 40;
    int fl = (int)(pct / 100.0 * bw);
    
    printf("\r[");
    for (int i = 0; i < bw; i++) {
        putchar(i < fl ? '=' : ' ');
    }
    printf("] %.1f%% | %ld/%ld | %lds | ETA %lds",
           pct, pb->cur, pb->tot, (long)el, (long)eta);
    
    if (pb->sh && pb->fn[0]) {
        printf(" | %s", pb->fn);
    }
    
    fflush(stdout);
}

void p_cmpl(Prog *pb) {
    if (!pb) return;
    pb->cur = pb->tot;
    p_dsp(pb);
    printf("\n");
    printf("Done in %lds\n", (long)p_el(pb));
}
