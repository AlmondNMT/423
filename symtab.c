#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"

extern tree_t* tree;

// Populate symbol tables from AST
void populate_symboltables(struct tree *t, SymbolTable st) {
    if (t == NULL) {
        return;
    }
    // In the simplest case, we just add NAME to the symbol table
    if(strcmp(t->symbolname, "funcdef") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text);
    } else if(strcmp(t->symbolname, "classdef") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text);
    } else if(strcmp(t->symbolname, "global_stmt") == 0) {
        SymbolTable global = get_global_symtab(st);
        add_global_names(global, t);
    }
    for(int i = 0; i < t->nkids; i++) {
        populate_symboltables(t->kids[i], st);
    }
}

/**
 * We're assuming that st is the global symbol table
 */
void add_global_names(SymbolTable st, tree_t *t)
{
    if(t->nkids == 0) {
        return;
    }
    if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text);
        add_global_names(st, t->kids[1]);
    }
    else {
        insertsymbol(st, t->kids[1]->leaf->text);
        add_global_names(st, t->kids[0]);
    }
}

SymbolTable get_global_symtab(SymbolTable st) 
{
    SymbolTable curr = st;
    while(curr->parent != NULL) {
        curr = curr->parent;
    }
    return curr;
}

void add_puny_builtins(SymbolTable st) {
    insertsymbol(st, "print");
    insertsymbol(st, "int");
    insertsymbol(st, "abs"); 
    insertsymbol(st, "bool");  
    insertsymbol(st, "chr");
    insertsymbol(st, "dict");
    insertsymbol(st, "float");
    insertsymbol(st, "input");
    insertsymbol(st, "int");    
    insertsymbol(st, "len");
    insertsymbol(st, "list");
    insertsymbol(st, "max");
    insertsymbol(st, "min");
    insertsymbol(st, "open");
    insertsymbol(st, "ord");
    insertsymbol(st, "pow");
    insertsymbol(st, "print");
    insertsymbol(st, "range");
    insertsymbol(st, "round");
    insertsymbol(st, "str");
    insertsymbol(st, "type");
}

int hash(SymbolTable st, char *s) {
    register unsigned int h = 0;
    register char c;
    while((c = *s++)) {
        h += c & 377;
        h *= 37;
    }
    return h % st->nBuckets;
}

// Create a new symbol table
SymbolTable mksymtab(int nbuckets)
{
    SymbolTable rv;
    rv = calloc(1, sizeof(struct sym_table));
    rv->tbl = calloc(nbuckets, sizeof(struct sym_entry *));
    rv->nBuckets = nbuckets;
    rv->nEntries = 0;
    rv->parent = NULL;
    return rv;
}

int insertsymbol(SymbolTable st, char *s) {
    int idx = hash(st, s);
    for (SymbolTableEntry e = st->tbl[idx]; e != NULL; e = e->next) {
        if (strcmp(e->s, s) == 0) {
            return 0;
        }
    }
    SymbolTableEntry entry = (SymbolTableEntry) malloc(sizeof(struct sym_entry));
    entry->table = st;
    entry->s = strdup(s);
    entry->next = st->tbl[idx];
    st->tbl[idx] = entry;
    st->nEntries++;
    return 1;
}



/*

void insertsymbol(SymbolTable *st, char *name, void *data) {
    int index = hash(name);
    SymbolTableEntry *entry = malloc(sizeof(SymbolTableEntry));
    entry->name = name;
    entry->data = data;
    entry->next = st->table[index];
    st->table[index] = entry;
}

*/

/*

// Remove a symbol from the symbol table
SymbolTableEntry removesymbol(SymbolTable *st, char *name) {
    int index = hash(name);
    SymbolTableEntry *prev = NULL;
    SymbolTableEntry *entry = st->table[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            if (prev == NULL) {
                st->table[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            return entry;
        }
        prev = entry;
        entry = entry->next;
    }
    return NULL;
}

*/


// Helper function to find a symbol in a symbol table
SymbolTableEntry findsymbol(SymbolTable st, char *s)
{
    int h;

    h = hash(st, s);
    for(SymbolTableEntry entry = st->tbl[h]; entry != NULL; entry = entry->next) {
        if(strcmp(s, entry->s) == 0) {
            /* Return a pointer to the symbol table entry. */
            return entry;
        }
    }
    return NULL;
}


////comment this out here for code to work uwu

void scope_enter(char *s) {
    SymbolTable *st = mksymtab();
    st -> level = (*s) -> level + 1;
    st -> parent = *s;
    *s = st;
}

void scope_exit(SymbolTable **s) {
    SymbolTable *st = *s;
    for (int i = 0; i < HASH_SIZE; i++) {
        SymbolTableEntry *entry = st->table[i];
        while (entry != NULL) {
            SymbolTableEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    *s = st->parent;
    free(st);
}

////comment this out here for code to work uwu

void printsymbols(SymbolTable st, int level)
{
    int i, j;
    if (st == NULL) return;
    for (i = 0; i < st->nBuckets; i++) {
        for(SymbolTableEntry entry = st->tbl[i]; entry != NULL; entry = entry->next) {
            printf("Symname: %s\n", entry->s);
        }
    }
}

/* TODO: Fix semantic errors funchy
void sematreeticerror(char *s, struct tree *tree)
{
    while(tree && (tree->nkids > 0)) tree = tree->u.kid[0];
    if(tree) {
        fpritreetf(stderr, "%s:%d: ", tree->u.leaf.filetreeame, tree->u.leaf.litreee);
    }
    fpritreetf(stderr, "%s", s);
    if(tree && tree->id == IDENT) fpritreetf(stderr, " %s", tree->u.leaf.text);
    fpritreetf(stderr, "\tree");
    errors++;
}


// Remove a symbol from the symbol table
SymbolTableEntry removesymbol(SymbolTable st, char *s) {
    SymbolTableEntry ste = st->next;
    SymbolTableEntry prev = NULL;
    int h = hash(st, s);
    while(ste != NULL) {
        if(strcmp(ste->s, s) == 0) {
            // Symbol found, remove it
            if(prev == NULL) {
                // Symbol is at the beginning of the list
                st->next = ste->next;
            } else {
                prev->next = ste->next;
            }
            st->nEntries--;
            return ste;
        }
        prev = ste;
        ste = ste->next;
    }

    // Symbol was not found
    return NULL;
}

// Print the symbol table
void printsymtab(SymbolTable st) {
    printf("Entries: %d \n", st->nEntries);
    SymbolTableEntry ste = st->;
    while(ste != NULL) {
        printf("\tSymbol: %s\n", ste->s);
        ste = ste->next;
    }
}*/

void free_symtab(SymbolTable st)
{

}






/*
idk man
prolly uncomment these in the future
lol


```
// Data structure for symbol table entry
typedef struct SymbolTableEntry {
    char *name;
    void *data;
    struct SymbolTableEntry *next;
} SymbolTableEntry;

// Data structure for symbol table
typedef struct SymbolTable {
    int level;
    SymbolTableEntry *table[HASH_SIZE];
    struct SymbolTable *parent;
} SymbolTable;

// Hash function
int hash(char *s) {
    int hashval = 0;
    for (; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % HASH_SIZE;
}

// Create a new symbol table
SymbolTable *mksymtab() {
    SymbolTable *st = malloc(sizeof(SymbolTable));
    st->level = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        st->table[i] = NULL;
    }
    st->parent = NULL;
    return st;
}

// Get the current scope level
int scope_level(SymbolTable *st) { return st->level;
}

// Look up a symbol in all scopes
void scope_lookup(SymbolTable *st, char *name) {
    SymbolTable *cur = st;
    while (cur != NULL) {
        int index = hash(name);
        SymbolTableEntry *entry = cur->table[index];
        while (entry != NULL) {
            if (strcmp(entry->name, name) == 0) {
                printf("Symbol found: %s\n", name);
                return;
            }
            entry = entry->next;
        }
        cur = cur->parent;
    }
    printf("Symbol not found: %s\n", name);
}

// Look up a symbol in the current scope only
void scope_lookup_current(SymbolTable *st, char *name) {
    int index = hash(name);
    SymbolTableEntry *entry = st->table[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            printf("Symbol found in current scope: %s\n", name);
            return;
        }
        entry = entry->next;
    }
    printf("Symbol not found in current scope: %s\n", name);
}

// Insert a symbol into the symbol table

// Find a symbol in the symbol table
SymbolTableEntry findsymbol(SymbolTable *st, char *name) {
    int index = hash(name);
    SymbolTableEntry *entry = st->table[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}
*/

