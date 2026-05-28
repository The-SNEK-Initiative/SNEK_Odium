#ifndef BAT_H
#define BAT_H

#include <stdio.h>
#include "prg.h"

typedef struct {
    char fn[512];
    int st;
    char err[256];
} Job;

typedef struct {
    Job *j;
    int c;
    int cap;
    int ci;
    Prog *p;
} Batch;

Batch* b_new(int cap);
void b_del(Batch *bp);
int b_add(Batch *bp, const char *fn);
int b_psrd(Batch *bp, int k);
int b_prs(Batch *bp, int k, const char *od);
void b_rpt(Batch *bp);
int b_gst(Batch *bp, int *tot, int *com, int *fai);

#endif
