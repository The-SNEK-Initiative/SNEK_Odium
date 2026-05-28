#ifndef HAS_H
#define HAS_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t s[4];
    uint32_t c[2];
    unsigned char b[64];
} MD5;

typedef struct {
    uint64_t h[8];
    uint8_t bu[128];
    uint32_t ln;
} SHA;

typedef struct {
    char md[33];
    char sh[65];
    unsigned long sz;
} HFile;

void m5_ini(MD5 *c);
void m5_upd(MD5 *c, const unsigned char *in, unsigned int ln);
void m5_fin(unsigned char *d, MD5 *c);
void m5_hex(unsigned char *d, char *h);
int m5_f(const char *fn, char *h);

void s2_ini(SHA *c);
void s2_upd(SHA *c, const uint8_t *da, uint32_t ln);
void s2_fin(SHA *c, uint8_t *d);
void s2_hex(uint8_t *d, char *h);
int s2_f(const char *fn, char *h);

int h_f(const char *fn, HFile *h);
int h_v(const char *fn, HFile *ex);

#endif
