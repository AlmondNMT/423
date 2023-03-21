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
}


/** 
 * Add print(), int(), str(), etc. to global symtab
 */
void add_puny_builtins(SymbolTable st) {
    insertsymbol(st, "print");
    insertsymbol(st, "int");
    insertsymbol(st, "abs");
    insertsymbol(st, "aiter");
    insertsymbol(st, "all");
    insertsymbol(st, "any");
    insertsymbol(st, "anext");
    insertsymbol(st, "ascii");
    insertsymbol(st, "bin");
    insertsymbol(st, "bool");
    insertsymbol(st, "breakpoint");
    insertsymbol(st, "bytearray");
    insertsymbol(st, "bytes");
    insertsymbol(st, "callable");
    insertsymbol(st, "chr");
    insertsymbol(st, "classmethod");
    insertsymbol(st, "compile");
    insertsymbol(st, "complex");
    insertsymbol(st, "delattr");
    insertsymbol(st, "dict");
    insertsymbol(st, "dir");
    insertsymbol(st, "divmod");
    insertsymbol(st, "enumerate");
    insertsymbol(st, "eval");
    insertsymbol(st, "exec");
    insertsymbol(st, "filter");
    insertsymbol(st, "float");
    insertsymbol(st, "format");
    insertsymbol(st, "frozenset");
    insertsymbol(st, "getattr");
    insertsymbol(st, "globals");
    insertsymbol(st, "hasattr");
    insertsymbol(st, "hash");
    insertsymbol(st, "help");
    insertsymbol(st, "hex");
    insertsymbol(st, "id");
    insertsymbol(st, "input");
    insertsymbol(st, "int");
    insertsymbol(st, "isinstance");
    insertsymbol(st, "issubclass");
    insertsymbol(st, "iter");
    insertsymbol(st, "len");
    insertsymbol(st, "list");
    insertsymbol(st, "locals");
    insertsymbol(st, "map");
    insertsymbol(st, "max");
    insertsymbol(st, "memoryview");
    insertsymbol(st, "min");
    insertsymbol(st, "next");
    insertsymbol(st, "object");
    insertsymbol(st, "oct");
    insertsymbol(st, "open");
    insertsymbol(st, "ord");
    insertsymbol(st, "pow");
    insertsymbol(st, "print");
    insertsymbol(st, "property");
    insertsymbol(st, "range");
    insertsymbol(st, "repr");
    insertsymbol(st, "reversed");
    insertsymbol(st, "round");
    insertsymbol(st, "set");
    insertsymbol(st, "setattr");
    insertsymbol(st, "slice");
    insertsymbol(st, "sorted");
    insertsymbol(st, "staticmethod");
    insertsymbol(st, "str");
    insertsymbol(st, "sum");
    insertsymbol(st, "super");
    insertsymbol(st, "tuple");
    insertsymbol(st, "type");
    insertsymbol(st, "vars");
    insertsymbol(st, "zip");
    insertsymbol(st, "__import__");
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

// 

// Insert a symbol into a symbol table.
// TODO: Fixing symbol insert
/*
int insertsymbol(SymbolTable st, char *s)
{
    int h;
    SymbolTableEntry se;
    h = hash(st, s);
    se = st->tbl[h];
    if(se == NULL) {
        st->tbl[h] = calloc(1, sizeof(struct sym_entr *));
        st->tbl[h]->s = s;
        st->nEntries++;
        return 1;
    }
    while(se->next != NULL) {
        if(se->s == s) {
            // Entry found return 0
            printf("%s\n", se->s);
            return 0;
        }
        se = se->next;
    }
    return 1;
} */
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

void scope_enter(char *s)
{
    /* allocate a new symbol table */
    /* insert s into current symbol table */
    /* attach new symbol table to s's symbol table in the current symbol table*/
    /* push new symbol on the stack, making it the current symbol table */
}

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

// Enter a new scope
void scope_enter(SymbolTable **pst) {
    SymbolTable *st = mksymtab();
    st->level = (*pst)->level + 1;
    st->parent = *pst;
    *pst = st;
}

// Exit the current scope
void scope_exit(SymbolTable **pst) {
    SymbolTable *st = *pst;
    for (int i = 0; i < HASH_SIZE; i++) {
        SymbolTableEntry *entry = st->table[i];
        while (entry != NULL) {
            SymbolTableEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    *pst = st->parent;
    free(st);
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
void insertsymbol(SymbolTable *st, char *name, void *data) {
    int index = hash(name);
    SymbolTableEntry *entry = malloc(sizeof(SymbolTableEntry));
    entry->name = name;
    entry->data = data;
    entry->next = st->table[index];
    st->table[index] = entry;
}
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
