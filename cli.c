#include "cli.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CLIArg* c_prs(int argc, char **argv) {
    CLIArg *args = malloc(sizeof(CLIArg));
    if (!args) return NULL;
    
    args->c = INT;
    memset(args->in, 0, sizeof(args->in));
    memset(args->od, 0, sizeof(args->od));
    memset(args->cf, 0, sizeof(args->cf));
    args->k = 0;
    args->v = 0;
    args->t = 4;
    args->vf = 1;
    args->cmp = 1;
    args->et = 2;
    
    if (argc < 2) {
        args->c = INT;
        return args;
    }
    
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        
        if (strcmp(arg, "--shard") == 0 && i + 2 < argc) {
            args->c = SHD;
            strncpy(args->in, argv[++i], sizeof(args->in) - 1);
            args->k = atoi(argv[++i]);
        } else if (strcmp(arg, "--restore") == 0 && i + 2 < argc) {
            args->c = RES;
            strncpy(args->in, argv[++i], sizeof(args->in) - 1);
            args->k = atoi(argv[++i]);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                strncpy(args->od, argv[++i], sizeof(args->od) - 1);
            }
        } else if (strcmp(arg, "--verify") == 0 && i + 1 < argc) {
            args->c = VER;
            strncpy(args->in, argv[++i], sizeof(args->in) - 1);
        } else if (strcmp(arg, "--list") == 0) {
            args->c = LST;
        } else if (strcmp(arg, "--batch-shard") == 0) {
            args->c = BSH;
            if (i + 1 < argc) {
                args->k = atoi(argv[++i]);
            }
        } else if (strcmp(arg, "--batch-restore") == 0) {
            args->c = BRS;
            if (i + 1 < argc) {
                args->k = atoi(argv[++i]);
            }
        } else if (strcmp(arg, "--genkey") == 0) {
            args->c = GEN;
        } else if (strcmp(arg, "--config") == 0 && i + 1 < argc) {
            strncpy(args->cf, argv[++i], sizeof(args->cf) - 1);
        } else if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0) {
            args->v++;
        } else if (strcmp(arg, "--threads") == 0 && i + 1 < argc) {
            args->t = atoi(argv[++i]);
        } else if (strcmp(arg, "--no-verify") == 0) {
            args->vf = 0;
        } else if (strcmp(arg, "--no-compress") == 0) {
            args->cmp = 0;
        } else if (strcmp(arg, "--info") == 0) {
            args->c = NFIO;
        } else if (strcmp(arg, "--version") == 0) {
            args->c = VRN;
        } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            args->c = HLP;
        }
    }
    
    return args;
}

void c_fr(CLIArg *args) {
    if (args) free(args);
}

void c_hlp(void) {
    printf("\n");
    printf("===== Odium File Sharding Tool =====\n\n");
    printf("USAGE:\n");
    printf("  odium [command] [options]\n\n");
    printf("COMMANDS:\n");
    printf("  --shard <file> <key>       Shard file\n");
    printf("  --restore <dir> <key>     Restore shards\n");
    printf("  --verify <file>            Verify file\n");
    printf("  --list                     List manifest\n");
    printf("  --batch-shard <key>        Batch shard\n");
    printf("  --batch-restore <key>      Batch restore\n");
    printf("  --genkey                   Generate key\n");
    printf("  --info                     Show info\n");
    printf("  --version                  Show version\n");
    printf("  --help                     Show help\n\n");
    printf("OPTIONS:\n");
    printf("  --config <file>            Load config\n");
    printf("  --verbose, -v              Verbose\n");
    printf("  --threads <n>              Threads\n");
    printf("  --no-verify                Skip verify\n");
    printf("  --no-compress              No compress\n\n");
}

void c_vrn(void) {
    printf("Odium - File Sharding Tool\n");
    printf("Version: 2.0.0\n");
    printf("Build: 2026-05-25\n\n");
}
