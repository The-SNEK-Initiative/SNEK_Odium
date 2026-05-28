#include "odium.h"

void ls(void) {
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

void gui(void) {
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
