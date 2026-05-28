#include "odium.h"

int shard(const char *src, int key) {
    FILE *fh = fopen(src, "rb");
    if (!fh) return 0;

    if (fseek(fh, 0, SEEK_END) != 0) { fclose(fh); return 0; }
    long sz = ftell(fh);
    if (sz < 0) { fclose(fh); return 0; }
    rewind(fh);

    unsigned char *raw = malloc((size_t)sz);
    if (!raw && sz) { fclose(fh); return 0; }
    if (fread(raw, 1, (size_t)sz, fh) != (size_t)sz) { fclose(fh); free(raw); return 0; }
    fclose(fh);

    char fid[IDL], d[512];
    gid(fid);
    
    if (strstr(p, VLT) == NULL) {
        edir(VLT);
        snprintf(d, sizeof(d), "%s/%s", VLT, fid);
    } else snprintf(d, sizeof(d), "%s", fid);
    MKD(d);

    int n = (rand() % (MAXN - MINN + 1)) + MINN;
    
    char mp[512];
    snprintf(mp, sizeof(mp), "%s/manifest.dat", d);
    FILE *fm = fopen(mp, "wb");
    if (!fm) { free(raw); return 0; }
    Meta m;
    memset(&m, 0, sizeof(Meta));
    strncpy(m.name, src, sizeof(m.name) - 1);
    m.n = n;
    m.key = key;
    m.sz = sz;
    if (fwrite(&m, sizeof(Meta), 1, fm) != 1) { fclose(fm); free(raw); return 0; }

    srand(key);
    int *idx = malloc((size_t)sz * sizeof(int));
    if (!idx && sz) { fclose(fm); free(raw); return 0; }
    for (long i = 0; i < sz; i++) idx[i] = (int)i;
    shuf(idx, (int)sz);

    unsigned char **dat = malloc((size_t)n * sizeof(unsigned char *));
    int *nsz = calloc((size_t)n, sizeof(int));
    if (!dat || !nsz) { fclose(fm); free(raw); free(idx); free(dat); free(nsz); return 0; }
    for (long i = 0; i < sz; i++) nsz[i % n]++;
    for (int i = 0; i < n; i++) {
        dat[i] = malloc((size_t)nsz[i]);
        if (!dat[i] && nsz[i]) { for (int j = 0; j < i; j++) free(dat[j]); fclose(fm); free(raw); free(idx); free(dat); free(nsz); return 0; }
        nsz[i] = 0;
    }

    for (long i = 0; i < sz; i++) {
        unsigned char rk = (unsigned char)((key + i) & 0xFF);
        dat[i % n][nsz[i % n]++] = raw[idx[i]] ^ rk;
    }

    for (int i = 0; i < n; i++) {
        char nid[IDL], np[512];
        gid(nid);
        snprintf(np, sizeof(np), "%s/%s", d, nid);
        MKD(np);
        fwrite(nid, IDL, 1, fm);

        int fc = (rand() % 3) + 2;
        int bp = nsz[i] / fc;
        int off = 0;

        for (int j = 0; j < fc; j++) {
            char gid_s[IDL], fp[512];
            gid(gid_s);
            snprintf(fp, sizeof(fp), "%s/%04d_%s%s", np, j, gid_s, EXT);
            int fsz = (j == fc - 1) ? (nsz[i] - off) : bp;
            FILE *fw = fopen(fp, "wb");
            if (fw) { fwrite(dat[i] + off, 1, (size_t)fsz, fw); fclose(fw); }
            off += fsz;
        }
        free(dat[i]);
    }
    fclose(fm);
    free(dat); free(nsz); free(idx); free(raw);
    return 1;
}

int back(const char *src, int pk, const char *dst) {
    char mp[512];
    snprintf(mp, sizeof(mp), "%s/manifest.dat", src);
    FILE *fm = fopen(mp, "rb");
    if (!fm) return 0;

    Meta m;
    if (fread(&m, sizeof(Meta), 1, fm) != 1) { fclose(fm); return 0; }
    if (m.key != pk) { fclose(fm); return -1; }

    unsigned char **dat = malloc((size_t)m.n * sizeof(unsigned char *));
    int *nsz = calloc((size_t)m.n, sizeof(int));
    if (!dat || !nsz) { fclose(fm); free(dat); free(nsz); return 0; }
    for (long i = 0; i < m.sz; i++) nsz[i % m.n]++;

    for (int i = 0; i < m.n; i++) {
        char nid[IDL], np[512];
        if (fread(nid, IDL, 1, fm) != 1) { for (int j = 0; j < i; j++) free(dat[j]); fclose(fm); free(dat); free(nsz); return 0; }
        dat[i] = malloc((size_t)nsz[i]);
        if (!dat[i] && nsz[i]) { for (int j = 0; j < i; j++) free(dat[j]); fclose(fm); free(dat); free(nsz); return 0; }
        int r = 0;
        snprintf(np, sizeof(np), "%s/%s", src, nid);
        
        for (int j = 0; ; j++) {
            DIR *dh = opendir(np);
            if (!dh) break;
            struct dirent *de;
            int ok = 0;
            char pre[10];
            snprintf(pre, sizeof(pre), "%04d_", j);
            while ((de = readdir(dh)) != NULL) {
                if (strncmp(de->d_name, pre, 5) == 0 && strstr(de->d_name, EXT)) {
                    char fp[512];
                    snprintf(fp, sizeof(fp), "%s/%s", np, de->d_name);
                    FILE *fr = fopen(fp, "rb");
                    if (fr) {
                        if (fseek(fr, 0, SEEK_END) == 0) {
                            long fs = ftell(fr);
                            if (fs >= 0) {
                                rewind(fr);
                                if (fread(dat[i] + r, 1, (size_t)fs, fr) == (size_t)fs) {
                                    r += (int)fs;
                                    ok = 1;
                                }
                            }
                        }
                        fclose(fr);
                    }
                    break;
                }
            }
            closedir(dh);
            if (!ok) break;
        }
    }
    fclose(fm);

    srand(m.key);
    int *idx = malloc((size_t)m.sz * sizeof(int));
    if (!idx && m.sz) { for (int i = 0; i < m.n; i++) free(dat[i]); free(dat); free(nsz); return 0; }
    for (long i = 0; i < m.sz; i++) idx[i] = (int)i;
    shuf(idx, (int)m.sz);

    unsigned char *out = malloc((size_t)m.sz);
    if (!out && m.sz) { free(idx); for (int i = 0; i < m.n; i++) free(dat[i]); free(dat); free(nsz); return 0; }
    int *cur = calloc((size_t)m.n, sizeof(int));
    if (!cur && m.n) { free(out); free(idx); for (int i = 0; i < m.n; i++) free(dat[i]); free(dat); free(nsz); return 0; }

    for (long i = 0; i < m.sz; i++) {
        unsigned char eb = dat[i % m.n][cur[i % m.n]++];
        unsigned char rk = (unsigned char)((m.key + i) & 0xFF);
        out[idx[i]] = eb ^ rk;
    }

    if (dst) {
        mkp(dst);
        char op[1024];
        const char *sl = strrchr(m.name, '/');
        if (!sl) sl = strrchr(m.name, '\\');
        snprintf(op, sizeof(op), "%s/restored_%s", dst, sl ? sl + 1 : m.name);
        FILE *fo = fopen(op, "wb");
        if (fo) { fwrite(out, 1, (size_t)m.sz, fo); fclose(fo); }
        if (isarc(op)) {
            if (unarc(op, dst)) remove(op);
        }
    }

    if (buf) free(buf);
    buf = out; bsz = m.sz;

    for (int i = 0; i < m.n; i++) free(dat[i]);
    free(dat); free(nsz); free(idx); free(cur);
    return 1;
}
