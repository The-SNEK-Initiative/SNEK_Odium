#include "odium.h"

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

void shuf(int *a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
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
