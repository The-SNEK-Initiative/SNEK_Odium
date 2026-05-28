#include "odium.h"
#include "log.h"
#include "cfg.h"
#include "has.h"
#include "cmp.h"
#include "prg.h"
#include "man.h"
#include "bat.h"
#include "cli.h"

Ent f[MAXF];
int cnt = 0;
int sel = 0;
char p[1024];
unsigned char *buf = NULL;
long bsz = 0;

int main(int ac, char *av[]) {
    srand(time(NULL));
    c_def();
    l_ini(LFILE, INF, 1);
    
    CLIArg *args = c_prs(ac, av);
    if (!args) {
        l_ftl("Parse failed");
        return 1;
    }
    
    if (args->cf[0]) {
        c_ld(args->cf);
    }
    
    glog.l = args->v;
    
    l_inf("Start");
    
    switch (args->c) {
        case SHD: {
            l_inf("Shard: %s", args->in);
            Prog *pb = p_new(1024 * 1024 * 100, 1);
            p_sfn(pb, args->in);
            
            if (shard(args->in, args->k)) {
                l_inf("Sharded");
                printf("OK\n");
            } else {
                l_err("Shard failed");
                printf("FAIL\n");
            }
            p_del(pb);
            break;
        }
        
        case RES: {
            l_inf("Restore: %s", args->in);
            Prog *pb = p_new(1024 * 1024 * 100, 1);
            p_sfn(pb, args->in);
            
            if (back(args->in, args->k, 
                    args->od[0] ? args->od : NULL)) {
                l_inf("Restored");
                printf("OK\n");
                if (args->od[0]) {
                    printf("Out: %s\n", args->od);
                }
            } else {
                l_err("Restore failed");
                printf("FAIL\n");
            }
            p_del(pb);
            break;
        }
        
        case VER: {
            l_inf("Verify: %s", args->in);
            HFile fh;
            if (h_f(args->in, &fh)) {
                printf("OK\n");
                printf("MD5: %s\n", fh.md);
                printf("SHA: %s\n", fh.sh);
                printf("Size: %lu\n", fh.sz);
            } else {
                l_err("Verify failed");
                printf("FAIL\n");
            }
            break;
        }
        
        case LST: {
            l_inf("Manifest");
            ManDB *db = m_new("odium.manifest");
            if (m_ld(db)) {
                m_lst(db);
            } else {
                printf("No manifest\n");
            }
            m_del(db);
            break;
        }
        
        case GEN: {
            int k = (int)time(NULL) ^ (rand() << 16);
            printf("Key: %d\n", k);
            l_inf("Key: %d", k);
            break;
        }
        
        case NFIO: {
            printf("\n=== Info ===\n");
            c_prt();
            break;
        }
        
        case VRN: {
            c_vrn();
            break;
        }
        
        case HLP: {
            c_hlp();
            break;
        }
        
        case INT: {
            l_inf("Interactive");
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
                                        Prog *pb = p_new(1024*1024*100, 1);
                                        shard(tmp, k);
                                        p_del(pb);
                                        remove(tmp);
                                    }
                                } else {
                                    Prog *pb = p_new(1024*1024*100, 1);
                                    shard(f[sel].name, k);
                                    p_del(pb);
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
                                    Prog *pb = p_new(1024*1024*100, 1);
                                    if (c == 1) back(f[sel].name, k, NULL);
                                    else { char o[512]; printf("Dest: "); scanf("%s", o); back(f[sel].name, k, o); }
                                    p_del(pb);
                                }
                                while(getchar() != '\n');
                            }
                            break;
                        case 8: CD(".."); GWD(p, sizeof(p)); sel = 0; ls(); break;
                    }
                }
            }
            break;
        }
        
        default: {
            printf("Unknown\n");
            c_hlp();
        }
    }
    
    if (buf) free(buf);
    c_fr(args);
    l_inf("Done");
    l_cls();
    
    return 0;
}
