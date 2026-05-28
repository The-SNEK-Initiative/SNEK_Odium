#ifndef CMP_H
#define CMP_H

#include <stdio.h>

typedef enum {
    NONE = 0,
    DEF = 1,
    GZ = 2,
    ZST = 3,
    LZ = 4
} ZType;

typedef struct {
    ZType t;
    int l;
    unsigned long os;
    unsigned long cs;
    float r;
} ZSt;

int z_f(const char *i, const char *o, ZType t, int l);
int z_d(const char *i, const char *o, ZType t);
int z_b(const unsigned char *i, size_t is,
        unsigned char **o, size_t *os, ZType t, int l);
int z_u(const unsigned char *i, size_t is,
        unsigned char **o, size_t *os, ZType t);
ZSt z_st(unsigned long o, unsigned long c);
void z_pt(ZSt *s);

#endif
