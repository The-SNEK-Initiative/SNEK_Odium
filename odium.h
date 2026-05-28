#ifndef ODIUM_H
#define ODIUM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "platform.h"
#include "log.h"
#include "cfg.h"
#include "has.h"
#include "cmp.h"
#include "prg.h"
#include "man.h"
#include "bat.h"
#include "cli.h"

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

extern Ent f[MAXF];
extern int cnt;
extern int sel;
extern char p[1024];
extern unsigned char *buf;
extern long bsz;

void gid(char *b);
int lcmp(const char *a, const char *b);
int isarc(const char *fn);
int fcopy(const char *src, const char *dst);
void shuf(int *a, int n);
void mkp(const char *path);
int edir(const char *path);

int arch(const char *src, char *out, size_t osz);
int unarc(const char *src, const char *dst);

int shard(const char *src, int key);
int back(const char *src, int pk, const char *dst);

void ls(void);
void gui(void);

#endif
