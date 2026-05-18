#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef _WIN32
    #include <direct.h>
    #include <conio.h>
    #define MKD(p) _mkdir(p)
    #define GWD _getcwd
    #define CD _chdir
#else
    #include <unistd.h>
    #include <termios.h>
    #define MKD(p) mkdir(p, 0777)
    #define GWD getcwd
    #define CD chdir
    int _getch() {
        struct termios o, n;
        int c;
        tcgetattr(0, &o);
        n = o;
        n.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &n);
        c = getchar();
        tcsetattr(0, TCSANOW, &o);
        return c;
    }
#endif

#define MAXN 10
#define MINN 4
#define EXT ".btc"
#define IDL 33
#define VLT "cry"
#define MAXF 1024
#define PLN 15
#define PWD 60

typedef struct {
    char name[256];
    int n;
    int key;
    long sz;
} Meta;

typedef struct {
    char name[256];
    int dir;
    long sz;
} Ent;

Ent f[MAXF];
int cnt = 0;
int sel = 0;
char p[1024];
unsigned char *buf = NULL;
long bsz = 0;

void gid(char *b) {
    const char *s = "0123456789abcdef";
    for (int i = 0; i < 32; i++) b[i] = s[rand() % 16];
    b[32] = '\0';
}

int lcmp(const char *a, const char *b) {
    while (*a && *b) {
        int x = (*a | 0x20);
        int y = (*b | 0x20);
        if (x != y) return x - y;
        a++; b++;
    }
    return *a - *b;
}

int isarc(const char *fn) {
    long l = (long)strlen(fn);
    if (l > 7 && !lcmp(fn + l - 7, ".tar.gz")) return 1;
    if (l > 4 && !lcmp(fn + l - 4, ".tgz")) return 1;
    if (l > 4 && (!lcmp(fn + l - 4, ".zip") || !lcmp(fn + l - 4, ".7z") || !lcmp(fn + l - 4, ".rar") || !lcmp(fn + l - 4, ".tar"))) return 1;
    return 0;
}

int fcopy(const char *src, const char *dst) {
    unsigned char buf[4096];
    FILE *fi = fopen(src, "rb");
    if (!fi) return 0;
    FILE *fo = fopen(dst, "wb");
    if (!fo) { fclose(fi); return 0; }
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fi)) > 0) fwrite(buf, 1, n, fo);
    fclose(fi); fclose(fo);
    return 1;
}

int arch(const char *src, char *out, size_t osz) {
    struct stat s;
    if (stat(src, &s) != 0) return 0;
    if (S_ISDIR(s.st_mode)) {
        snprintf(out, osz, ".odiuma.%ld.tar", time(NULL));
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "tar -cf \"%s\" -C \"%s\" .", out, src);
        return system(cmd) == 0;
    }
    if (isarc(src)) {
        return fcopy(src, out);
    }
    return 0;
}

void mkp(const char *path);

int unarc(const char *src, const char *dst) {
    char cmd[1024];
    long l = (long)strlen(src);
    mkp(dst);
    if (l > 7 && !lcmp(src + l - 7, ".tar.gz")) {
        snprintf(cmd, sizeof(cmd), "tar -xf \"%s\" -C \"%s\"", src, dst);
        return system(cmd) == 0;
    }
    if (l > 4 && !lcmp(src + l - 4, ".tgz")) {
        snprintf(cmd, sizeof(cmd), "tar -xf \"%s\" -C \"%s\"", src, dst);
        return system(cmd) == 0;
    }
    if (l > 4 && !lcmp(src + l - 4, ".tar")) {
        snprintf(cmd, sizeof(cmd), "tar -xf \"%s\" -C \"%s\"", src, dst);
        return system(cmd) == 0;
    }
    if (l > 4 && !lcmp(src + l - 4, ".zip")) {
        snprintf(cmd, sizeof(cmd), "unzip -qq \"%s\" -d \"%s\"", src, dst);
        return system(cmd) == 0;
    }
    if (l > 3 && !lcmp(src + l - 3, ".7z")) {
        snprintf(cmd, sizeof(cmd), "7z x -y -o\"%s\" \"%s\"", dst, src);
        return system(cmd) == 0;
    }
    if (l > 4 && !lcmp(src + l - 4, ".rar")) {
        snprintf(cmd, sizeof(cmd), "unrar x -y \"%s\" \"%s\"", src, dst);
        return system(cmd) == 0;
    }
    return 0;
}

int edir(const char *path) {
    struct stat s = {0};
    if (stat(path, &s) == -1) return MKD(path);
    return 0;
}

void mkp(const char *path) {
    char t[1024];
    char *q = NULL;
    snprintf(t, sizeof(t), "%s", path);
    for (q = t + 1; *q; q++) {
        if (*q == '/' || *q == '\\') {
            char s = *q;
            *q = 0;
            edir(t);
            *q = s;
        }
    }
    edir(t);
}

void shuf(int *a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
}

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

void ls() {
    DIR *dh = opendir(".");
    struct dirent *de;
    cnt = 0;
    if (dh) {
        while ((de = readdir(dh)) != NULL && cnt < MAXF) {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
            strcpy(f[cnt].name, de->d_name);
            struct stat s;
            if (stat(de->d_name, &s) == 0) {
                f[cnt].dir = S_ISDIR(s.st_mode);
                f[cnt].sz = s.st_size;
            } else {
                f[cnt].dir = 0;
                f[cnt].sz = 0;
            }
            cnt++;
        }
        closedir(dh);
    }
}

void gui() {
    printf("\033[H\033[J\033[1;32m SNEK Odium \033[0m | %s\n", p);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < cnt; i++) {
        if (i == sel) printf("\033[7m"); 
        if (f[i].dir) printf(" [DIR] %-30s ", f[i].name);
        else printf("       %-30s %10ld b ", f[i].name, f[i].sz);
        printf("\033[0m\n");
    }
    printf("\n\033[1;34m-- PREVIEW --\033[0m\n");
    if (buf) {
        printf("\033[1;35m[DECRYPTED]\033[0m\n");
        for (int i = 0; i < bsz && i < (PLN * PWD); i++) {
            unsigned char c = buf[i];
            if (c >= 32 && c <= 126) putchar(c);
            else if (c == '\n') putchar(c); else putchar('.');
            if (i % PWD == PWD - 1) printf("\n");
        }
    } else if (!f[sel].dir) {
        FILE *fh = fopen(f[sel].name, "rb");
        if (fh) {
            char l[PWD]; int ln = 0;
            while (fgets(l, PWD, fh) && ln < PLN) { printf("  %s", l); ln++; }
            fclose(fh);
        }
    } else printf("  [Dir]\n");
    printf("\n\033[1;33m[S] Shard | [R] Restore | [X] Clear | [Enter]/Right Go | Left/Backspace Back | [Q] Quit\033[0m\n");
}

int main(int ac, char *av[]) {
    srand(time(NULL));
    if (ac > 1) {
        if (strcmp(av[1], "--shard") == 0 && ac == 4) return shard(av[2], atoi(av[3])) ? 0 : 1;
        if (strcmp(av[1], "--restr") == 0 && ac == 5) return (back(av[2], atoi(av[4]), av[3]) == 1) ? 0 : 1;
        return 0;
    }
    GWD(p, sizeof(p));
    ls();
    int run = 1;
    while (run) {
        gui();
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            ch = _getch();
            if (ch == 72 && sel > 0) sel--;
            if (ch == 80 && sel < cnt - 1) sel++;
            if (ch == 77 && f[sel].dir) { CD(f[sel].name); GWD(p, sizeof(p)); sel = 0; ls(); }
            if (ch == 75) { CD(".."); GWD(p, sizeof(p)); sel = 0; ls(); }
        } else {
            switch (ch) {
                case 'q': case 'Q': run = 0; break;
                case 'x': case 'X': if (buf) { free(buf); buf = NULL; } break;
                case 13: if (f[sel].dir) { CD(f[sel].name); GWD(p, sizeof(p)); sel = 0; ls(); } break;
                case 's': case 'S': {
                    printf("\nKey: "); int k;
                    if (scanf("%d", &k) == 1) {
                        if (f[sel].dir) {
                            char tmp[1024];
                            if (arch(f[sel].name, tmp, sizeof(tmp))) {
                                shard(tmp, k);
                                remove(tmp);
                            }
                        } else {
                            shard(f[sel].name, k);
                        }
                    }
                    while(getchar() != '\n');
                    ls();
                } break;
                case 'r': case 'R':
                    if (f[sel].dir) {
                        printf("\nKey: "); int k;
                        if (scanf("%d", &k) == 1) {
                            printf("\n[1] View [2] Save: "); int c; scanf("%d", &c);
                            if (c == 1) back(f[sel].name, k, NULL);
                            else { char o[512]; printf("Dest: "); scanf("%s", o); back(f[sel].name, k, o); }
                        }
                        while(getchar() != '\n');
                    }
                    break;
                case 8: CD(".."); GWD(p, sizeof(p)); sel = 0; ls(); break;
            }
        }
    }
    if (buf) free(buf);
    return 0;
}