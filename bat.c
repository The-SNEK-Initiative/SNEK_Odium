#include "bat.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

Batch* b_new(int cap) {
    Batch *bp = malloc(sizeof(Batch));
    if (!bp) return NULL;
    
    bp->j = malloc(sizeof(Job) * cap);
    if (!bp->j) {
        free(bp);
        return NULL;
    }
    
    bp->c = 0;
    bp->cap = cap;
    bp->ci = 0;
    bp->p = p_new(0, 1);
    
    return bp;
}

void b_del(Batch *bp) {
    if (bp) {
        if (bp->j) free(bp->j);
        if (bp->p) p_del(bp->p);
        free(bp);
    }
}

int b_add(Batch *bp, const char *fn) {
    if (!bp || !fn) return 0;
    
    if (bp->c >= bp->cap) {
        l_wrn("Batch full");
        return 0;
    }
    
    Job *job = &bp->j[bp->c];
    strncpy(job->fn, fn, sizeof(job->fn) - 1);
    job->fn[sizeof(job->fn) - 1] = '\0';
    job->st = 0;
    memset(job->err, 0, sizeof(job->err));
    
    bp->c++;
    l_inf("Added: %s", fn);
    return 1;
}

int b_psrd(Batch *bp, int k) {
    if (!bp || bp->c == 0) return 0;
    
    l_inf("Batch shard: %d files", bp->c);
    
    for (int i = 0; i < bp->c; i++) {
        Job *job = &bp->j[i];
        job->st = 1;
        
        struct stat st;
        if (stat(job->fn, &st) != 0) {
            job->st = 3;
            snprintf(job->err, sizeof(job->err), "Not found");
            l_err("Job failed: %s", job->fn);
            continue;
        }
        
        p_sfn(bp->p, job->fn);
        p_upd(bp->p, st.st_size * (i + 1) / bp->c);
        p_dsp(bp->p);
        
        job->st = 2;
        l_inf("Done: %s", job->fn);
    }
    
    p_cmpl(bp->p);
    return 1;
}

int b_prs(Batch *bp, int k, const char *od) {
    if (!bp || bp->c == 0 || !od) return 0;
    
    l_inf("Batch restore: %d dirs", bp->c);
    
    for (int i = 0; i < bp->c; i++) {
        Job *job = &bp->j[i];
        job->st = 1;
        
        struct stat st;
        if (stat(job->fn, &st) != 0 || !S_ISDIR(st.st_mode)) {
            job->st = 3;
            snprintf(job->err, sizeof(job->err), "Bad dir");
            l_err("Job failed: %s", job->fn);
            continue;
        }
        
        p_sfn(bp->p, job->fn);
        p_upd(bp->p, (i + 1) * 100 / bp->c);
        p_dsp(bp->p);
        
        job->st = 2;
        l_inf("Done: %s", job->fn);
    }
    
    p_cmpl(bp->p);
    return 1;
}

void b_rpt(Batch *bp) {
    if (!bp) return;
    
    int tot, com, fai;
    b_gst(bp, &tot, &com, &fai);
    
    printf("\n");
    printf("=== Batch Report ===\n");
    printf("Total: %d\n", tot);
    printf("Done: %d (%.1f%%)\n", com, 100.0 * com / tot);
    printf("Failed: %d\n", fai);
    printf("====================\n\n");
    
    if (fai > 0) {
        printf("Failed:\n");
        for (int i = 0; i < bp->c; i++) {
            if (bp->j[i].st == 3) {
                printf("  - %s: %s\n", bp->j[i].fn, bp->j[i].err);
            }
        }
        printf("\n");
    }
}

int b_gst(Batch *bp, int *tot, int *com, int *fai) {
    if (!bp) return 0;
    
    *tot = bp->c;
    *com = 0;
    *fai = 0;
    
    for (int i = 0; i < bp->c; i++) {
        if (bp->j[i].st == 2) (*com)++;
        else if (bp->j[i].st == 3) (*fai)++;
    }
    
    return 1;
}
