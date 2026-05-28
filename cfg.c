#include "cfg.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

Cfg gcfg = {0};

void c_def(void) {
    gcfg.sn = 4;
    gcfg.sx = 10;
    gcfg.rd = 1;
    gcfg.zc = 1;
    gcfg.zl = 6;
    gcfg.ec = 1;
    gcfg.et = 2;
    gcfg.sd = 0;
    gcfg.pt = 4;
    gcfg.cs = 1024 * 1024;
    gcfg.bs = 64 * 1024 * 1024;
    gcfg.vc = 1;
    strcpy(gcfg.lf, "odium.log");
    strcpy(gcfg.td, ".odium_tmp");
    gcfg.ll = 1;
}

int c_ld(const char *fn) {
    FILE *f = fopen(fn, "r");
    if (!f) {
        l_wrn("Config not found: %s", fn);
        return 0;
    }
    
    char ln[512];
    while (fgets(ln, sizeof(ln), f)) {
        if (ln[0] == '#' || ln[0] == '\n') continue;
        
        char k[256], v[256];
        if (sscanf(ln, "%255s = %255s", k, v) != 2) continue;
        
        if (strcmp(k, "shard_min") == 0) gcfg.sn = atoi(v);
        else if (strcmp(k, "shard_max") == 0) gcfg.sx = atoi(v);
        else if (strcmp(k, "redundancy_level") == 0) gcfg.rd = atoi(v);
        else if (strcmp(k, "use_compression") == 0) gcfg.zc = atoi(v);
        else if (strcmp(k, "compression_level") == 0) gcfg.zl = atoi(v);
        else if (strcmp(k, "use_encryption") == 0) gcfg.ec = atoi(v);
        else if (strcmp(k, "encryption_type") == 0) gcfg.et = atoi(v);
        else if (strcmp(k, "secure_delete") == 0) gcfg.sd = atoi(v);
        else if (strcmp(k, "parallel_threads") == 0) gcfg.pt = atoi(v);
        else if (strcmp(k, "chunk_size") == 0) gcfg.cs = atoi(v);
        else if (strcmp(k, "buffer_size") == 0) gcfg.bs = atoi(v);
        else if (strcmp(k, "verify_checksums") == 0) gcfg.vc = atoi(v);
        else if (strcmp(k, "log_file") == 0) strcpy(gcfg.lf, v);
        else if (strcmp(k, "temp_dir") == 0) strcpy(gcfg.td, v);
        else if (strcmp(k, "log_level") == 0) gcfg.ll = atoi(v);
    }
    
    fclose(f);
    l_inf("Config loaded: %s", fn);
    return 1;
}

int c_sv(const char *fn) {
    FILE *f = fopen(fn, "w");
    if (!f) {
        l_err("Failed to save config: %s", fn);
        return 0;
    }
    
    fprintf(f, "shard_min = %d\n", gcfg.sn);
    fprintf(f, "shard_max = %d\n", gcfg.sx);
    fprintf(f, "redundancy_level = %d\n", gcfg.rd);
    fprintf(f, "use_compression = %d\n", gcfg.zc);
    fprintf(f, "compression_level = %d\n", gcfg.zl);
    fprintf(f, "use_encryption = %d\n", gcfg.ec);
    fprintf(f, "encryption_type = %d\n", gcfg.et);
    fprintf(f, "secure_delete = %d\n", gcfg.sd);
    fprintf(f, "parallel_threads = %d\n", gcfg.pt);
    fprintf(f, "chunk_size = %d\n", gcfg.cs);
    fprintf(f, "buffer_size = %d\n", gcfg.bs);
    fprintf(f, "verify_checksums = %d\n", gcfg.vc);
    fprintf(f, "log_file = %s\n", gcfg.lf);
    fprintf(f, "temp_dir = %s\n", gcfg.td);
    fprintf(f, "log_level = %d\n", gcfg.ll);
    
    fclose(f);
    l_inf("Config saved: %s", fn);
    return 1;
}

void c_prt(void) {
    printf("\n=== Config ===\n");
    printf("Shard Min: %d\n", gcfg.sn);
    printf("Shard Max: %d\n", gcfg.sx);
    printf("Redundancy: %d\n", gcfg.rd);
    printf("Compression: %s\n", gcfg.zc ? "YES" : "NO");
    printf("Encrypt: %s\n", gcfg.ec ? "YES" : "NO");
    printf("Threads: %d\n", gcfg.pt);
    printf("Verify: %s\n", gcfg.vc ? "YES" : "NO");
    printf("=============\n\n");
}
