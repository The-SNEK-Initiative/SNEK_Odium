#include "man.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ManDB* m_new(const char *db) {
    ManDB *mdb = malloc(sizeof(ManDB));
    if (!mdb) return NULL;
    
    mdb->e = malloc(sizeof(MEntry) * 100);
    if (!mdb->e) {
        free(mdb);
        return NULL;
    }
    
    mdb->c = 0;
    mdb->cap = 100;
    strncpy(mdb->db, db, sizeof(mdb->db) - 1);
    mdb->db[sizeof(mdb->db) - 1] = '\0';
    
    return mdb;
}

void m_del(ManDB *db) {
    if (db) {
        if (db->e) free(db->e);
        free(db);
    }
}

int m_add(ManDB *db, MEntry *en) {
    if (!db || !en) return 0;
    
    if (db->c >= db->cap) {
        db->cap *= 2;
        MEntry *ne = realloc(db->e, sizeof(MEntry) * db->cap);
        if (!ne) {
            l_err("Expand failed");
            return 0;
        }
        db->e = ne;
    }
    
    memcpy(&db->e[db->c], en, sizeof(MEntry));
    db->c++;
    l_inf("Entry added: %s", en->fn);
    return 1;
}

int m_fnd(ManDB *db, const char *fn, MEntry *en) {
    if (!db || !fn || !en) return 0;
    
    for (int i = 0; i < db->c; i++) {
        if (strcmp(db->e[i].fn, fn) == 0) {
            memcpy(en, &db->e[i], sizeof(MEntry));
            return 1;
        }
    }
    
    return 0;
}

int m_ld(ManDB *db) {
    if (!db) return 0;
    
    FILE *f = fopen(db->db, "rb");
    if (!f) {
        l_wrn("Manifest not found: %s", db->db);
        return 0;
    }
    
    int cnt;
    if (fread(&cnt, sizeof(int), 1, f) != 1) {
        l_err("Read count failed");
        fclose(f);
        return 0;
    }
    
    if (cnt > db->cap) {
        db->cap = cnt + 50;
        MEntry *ne = realloc(db->e, sizeof(MEntry) * db->cap);
        if (!ne) {
            l_err("Alloc failed");
            fclose(f);
            return 0;
        }
        db->e = ne;
    }
    
    for (int i = 0; i < cnt; i++) {
        if (fread(&db->e[i], sizeof(MEntry), 1, f) != 1) {
            l_err("Read entry failed");
            fclose(f);
            return 0;
        }
    }
    db->c = cnt;
    
    fclose(f);
    l_inf("Loaded %d entries from %s", cnt, db->db);
    return 1;
}

int m_sv(ManDB *db) {
    if (!db) return 0;
    
    FILE *f = fopen(db->db, "wb");
    if (!f) {
        l_err("Cannot write: %s", db->db);
        return 0;
    }
    
    if (fwrite(&db->c, sizeof(int), 1, f) != 1) {
        l_err("Write count failed");
        fclose(f);
        return 0;
    }
    
    for (int i = 0; i < db->c; i++) {
        if (fwrite(&db->e[i], sizeof(MEntry), 1, f) != 1) {
            l_err("Write entry failed");
            fclose(f);
            return 0;
        }
    }
    
    fclose(f);
    l_inf("Saved %d entries to %s", db->c, db->db);
    return 1;
}

int m_lst(ManDB *db) {
    if (!db || db->c == 0) {
        printf("No entries.\n");
        return 0;
    }
    
    printf("\n=== Manifest (%d) ===\n\n", db->c);
    for (int i = 0; i < db->c; i++) {
        m_prt(&db->e[i]);
        printf("\n");
    }
    return 1;
}

void m_prt(MEntry *en) {
    if (!en) return;
    
    printf("File: %s\n", en->fn);
    printf("Size: %lu bytes\n", en->sz);
    printf("MD5: %s\n", en->md);
    printf("SHA: %s\n", en->sh);
    printf("Shards: %d\n", en->sc);
    printf("Redundancy: %d\n", en->rd);
    printf("Status: %s\n", en->st == 0 ? "OK" : (en->st == 1 ? "PARTIAL" : "BAD"));
}
