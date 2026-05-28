#include "cmp.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int s_cmp(const unsigned char *i, size_t is,
          unsigned char **o, size_t *os) {
    *o = malloc(is * 2);
    if (!*o) return 0;
    
    unsigned char *out = *o;
    size_t pos = 0;
    
    for (size_t j = 0; j < is; ) {
        unsigned char by = i[j];
        size_t cnt = 1;
        
        while (j + cnt < is && i[j + cnt] == by && cnt < 255) {
            cnt++;
        }
        
        if (cnt >= 4) {
            out[pos++] = 0xFF;
            out[pos++] = by;
            out[pos++] = cnt;
            j += cnt;
        } else {
            for (size_t k = 0; k < cnt; k++) {
                out[pos++] = by;
            }
            j += cnt;
        }
        
        if (pos >= is) {
            *os = is * 2;
            return 1;
        }
    }
    
    *os = pos;
    return 1;
}

int s_dcmp(const unsigned char *i, size_t is,
           unsigned char **o, size_t *os) {
    *o = malloc(is * 4);
    if (!*o) return 0;
    
    unsigned char *out = *o;
    size_t pos = 0;
    
    for (size_t j = 0; j < is; ) {
        if (i[j] == 0xFF && j + 2 < is) {
            unsigned char by = i[j + 1];
            unsigned char cnt = i[j + 2];
            for (int k = 0; k < cnt; k++) {
                out[pos++] = by;
            }
            j += 3;
        } else {
            out[pos++] = i[j];
            j++;
        }
    }
    
    *os = pos;
    return 1;
}

int z_f(const char *i, const char *o, ZType t, int l) {
    FILE *fi = fopen(i, "rb");
    if (!fi) {
        l_err("Cannot open: %s", i);
        return 0;
    }
    
    fseek(fi, 0, SEEK_END);
    size_t sz = ftell(fi);
    rewind(fi);
    
    unsigned char *d = malloc(sz);
    if (!d) {
        l_err("Alloc failed");
        fclose(fi);
        return 0;
    }
    
    if (fread(d, 1, sz, fi) != sz) {
        l_err("Read failed");
        fclose(fi);
        free(d);
        return 0;
    }
    fclose(fi);
    
    unsigned char *cmp = NULL;
    size_t csz = 0;
    
    switch (t) {
        case NONE:
            cmp = d;
            csz = sz;
            break;
        case DEF:
        case GZ:
        case ZST:
        case LZ:
            if (!s_cmp(d, sz, &cmp, &csz)) {
                l_err("Compress failed");
                free(d);
                return 0;
            }
            break;
    }
    
    FILE *fo = fopen(o, "wb");
    if (!fo) {
        l_err("Cannot open: %s", o);
        if (cmp != d) free(cmp);
        free(d);
        return 0;
    }
    
    if (fwrite(&t, sizeof(t), 1, fo) != 1 ||
        fwrite(&sz, sizeof(sz), 1, fo) != 1 ||
        fwrite(cmp, 1, csz, fo) != csz) {
        l_err("Write failed");
        fclose(fo);
        if (cmp != d) free(cmp);
        free(d);
        return 0;
    }
    
    fclose(fo);
    if (cmp != d) free(cmp);
    free(d);
    
    l_inf("Compressed: %s -> %s", i, o);
    return 1;
}

int z_d(const char *i, const char *o, ZType t) {
    FILE *fi = fopen(i, "rb");
    if (!fi) {
        l_err("Cannot open: %s", i);
        return 0;
    }
    
    ZType ft;
    size_t os;
    
    if (fread(&ft, sizeof(ft), 1, fi) != 1 ||
        fread(&os, sizeof(os), 1, fi) != 1) {
        l_err("Read header failed");
        fclose(fi);
        return 0;
    }
    
    fseek(fi, 0, SEEK_END);
    size_t fsz = ftell(fi) - sizeof(ZType) - sizeof(size_t);
    rewind(fi);
    fseek(fi, sizeof(ZType) + sizeof(size_t), SEEK_SET);
    
    unsigned char *cmp = malloc(fsz);
    if (!cmp) {
        l_err("Alloc failed");
        fclose(fi);
        return 0;
    }
    
    if (fread(cmp, 1, fsz, fi) != fsz) {
        l_err("Read failed");
        fclose(fi);
        free(cmp);
        return 0;
    }
    fclose(fi);
    
    unsigned char *dcmp = NULL;
    size_t dsz = 0;
    
    if (!s_dcmp(cmp, fsz, &dcmp, &dsz)) {
        l_err("Decompress failed");
        free(cmp);
        return 0;
    }
    
    FILE *fo = fopen(o, "wb");
    if (!fo) {
        l_err("Cannot open: %s", o);
        free(dcmp);
        free(cmp);
        return 0;
    }
    
    if (fwrite(dcmp, 1, dsz, fo) != dsz) {
        l_err("Write failed");
        fclose(fo);
        free(dcmp);
        free(cmp);
        return 0;
    }
    
    fclose(fo);
    free(dcmp);
    free(cmp);
    
    l_inf("Decompressed: %s -> %s", i, o);
    return 1;
}

int z_b(const unsigned char *i, size_t is,
        unsigned char **o, size_t *os, ZType t, int l) {
    if (t == NONE) {
        *o = malloc(is);
        if (!*o) return 0;
        memcpy(*o, i, is);
        *os = is;
        return 1;
    }
    
    return s_cmp(i, is, o, os);
}

int z_u(const unsigned char *i, size_t is,
        unsigned char **o, size_t *os, ZType t) {
    if (t == NONE) {
        *o = malloc(is);
        if (!*o) return 0;
        memcpy(*o, i, is);
        *os = is;
        return 1;
    }
    
    return s_dcmp(i, is, o, os);
}

ZSt z_st(unsigned long o, unsigned long c) {
    ZSt s;
    s.os = o;
    s.cs = c;
    s.r = o > 0 ? (100.0 * c / o) : 0;
    return s;
}

void z_pt(ZSt *s) {
    printf("Compression:\n");
    printf("  Original: %lu bytes\n", s->os);
    printf("  Compressed: %lu bytes\n", s->cs);
    printf("  Ratio: %.2f%%\n", s->r);
    printf("  Saved: %lu bytes\n", s->os - s->cs);
}
