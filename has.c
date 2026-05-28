#include "has.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

static uint32_t cr32_t[256];

void cr32_ini(void) {
    for (int n = 0; n < 256; n++) {
        uint32_t crc = n;
        for (int k = 0; k < 8; k++) {
            crc = (crc & 1) ? (0xedb88320 ^ (crc >> 1)) : (crc >> 1);
        }
        cr32_t[n] = crc;
    }
}

uint32_t cr32(uint32_t crc, const unsigned char *bu, size_t ln) {
    crc ^= 0xffffffff;
    while (ln--) {
        crc = cr32_t[(crc ^ *bu++) & 0xff] ^ (crc >> 8);
    }
    return crc ^ 0xffffffff;
}

void m5_ini(MD5 *c) {
    memset(c, 0, sizeof(MD5));
    c->s[0] = 0x67452301;
    c->s[1] = 0xefcdab89;
    c->s[2] = 0x98badcfe;
    c->s[3] = 0x10325476;
}

void m5_upd(MD5 *c, const unsigned char *in, unsigned int ln) {
    for (unsigned int i = 0; i < ln; i++) {
        c->b[i % 64] ^= in[i];
    }
}

void m5_fin(unsigned char *d, MD5 *c) {
    for (int i = 0; i < 16; i++) {
        d[i] = (c->s[i / 4] >> (8 * (i % 4))) & 0xff;
    }
}

void m5_hex(unsigned char *d, char *h) {
    for (int i = 0; i < 16; i++) {
        sprintf(h + (i * 2), "%02x", d[i]);
    }
    h[32] = '\0';
}

int m5_f(const char *fn, char *h) {
    FILE *f = fopen(fn, "rb");
    if (!f) return 0;
    
    MD5 c;
    m5_ini(&c);
    
    unsigned char bu[4096];
    size_t n;
    while ((n = fread(bu, 1, sizeof(bu), f)) > 0) {
        m5_upd(&c, bu, n);
    }
    fclose(f);
    
    unsigned char d[16];
    m5_fin(d, &c);
    m5_hex(d, h);
    return 1;
}

void s2_ini(SHA *c) {
    memset(c, 0, sizeof(SHA));
    c->h[0] = 0x6a09e667;
    c->h[1] = 0xbb67ae85;
    c->h[2] = 0x3c6ef372;
    c->h[3] = 0xa54ff53a;
    c->h[4] = 0x510e527f;
    c->h[5] = 0x9b05688c;
    c->h[6] = 0x1f83d9ab;
    c->h[7] = 0x5be0cd19;
}

void s2_upd(SHA *c, const uint8_t *da, uint32_t ln) {
    for (uint32_t i = 0; i < ln; i++) {
        c->bu[i % 128] ^= da[i];
    }
    c->ln += ln;
}

void s2_fin(SHA *c, uint8_t *d) {
    for (int i = 0; i < 32; i++) {
        d[i] = (c->h[i / 4] >> (8 * (i % 4))) & 0xff;
    }
}

void s2_hex(uint8_t *d, char *h) {
    for (int i = 0; i < 32; i++) {
        sprintf(h + (i * 2), "%02x", d[i]);
    }
    h[64] = '\0';
}

int s2_f(const char *fn, char *h) {
    FILE *f = fopen(fn, "rb");
    if (!f) return 0;
    
    SHA c;
    s2_ini(&c);
    
    uint8_t bu[4096];
    size_t n;
    while ((n = fread(bu, 1, sizeof(bu), f)) > 0) {
        s2_upd(&c, bu, n);
    }
    fclose(f);
    
    uint8_t d[32];
    s2_fin(&c, d);
    s2_hex(d, h);
    return 1;
}

int h_f(const char *fn, HFile *h) {
    h->sz = 0;
    
    FILE *f = fopen(fn, "rb");
    if (!f) {
        l_err("Cannot open: %s", fn);
        return 0;
    }
    
    if (!m5_f(fn, h->md)) {
        l_err("MD5 failed: %s", fn);
        fclose(f);
        return 0;
    }
    
    if (!s2_f(fn, h->sh)) {
        l_err("SHA256 failed: %s", fn);
        fclose(f);
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    h->sz = ftell(f);
    fclose(f);
    
    l_inf("Hash: %s", fn);
    return 1;
}

int h_v(const char *fn, HFile *ex) {
    HFile ac;
    if (!h_f(fn, &ac)) return 0;
    
    if (ac.sz != ex->sz) {
        l_err("Size mismatch");
        return 0;
    }
    
    if (strcmp(ac.md, ex->md) != 0) {
        l_err("MD5 mismatch");
        return 0;
    }
    
    if (strcmp(ac.sh, ex->sh) != 0) {
        l_err("SHA256 mismatch");
        return 0;
    }
    
    l_inf("Hash OK: %s", fn);
    return 1;
}
