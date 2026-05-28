#include "odium.h"

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
