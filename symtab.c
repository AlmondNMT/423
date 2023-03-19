#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"

extern YYSTYPE yylval;

void populate_symboltables(struct tree *t, SymbolTable st)
{
    if(t == NULL) return;
    /*switch(t->prodrule) {
    case yylval.global_stmt: 
        // Need to iterate through all comma-separated names in the global statement 
        break;
    }*/
}

// Return an index to the hash table
int hash(SymbolTable st, char *s)
{
    register int h = 0;
    register char c;
    while((c = *s++)) {
        h += c & 377;
        h *= 37;
    }
    if(h < 0) h = -h;
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
    return rv;
}

// Insert a symbol into a symbol table.
int insertsymbol(SymbolTable st, char *s)
{
    int h;
    SymbolTableEntry se;
    h = hash(st, s);
    for(se = st->tbl[h]; se != NULL; se = se->next) {
        if(!strcmp(s, se->s)) { 
            /* A copy of the string is already in the table. */
            return 0;
        }
    }
    /*
     * The string is not in the table. Add the copy from the 
     * buffer to the table.
     */
    se = calloc(1, sizeof(struct sym_entry));
    se->next = st->tbl[h];
    se->table = st;
    st->tbl[h] = se;
    se->s = strdup(s);
    //se->type = t;
    st->nEntries++;
    return 1;
}

// Helper function to find a symbol in a symbol table
SymbolTableEntry findsymbol(SymbolTable st, char *s)
{
    int h;
    SymbolTableEntry se;

    h = hash(st, s);
    for(se = st->tbl[h]; se != NULL; se = se->next) {
        if(!strcmp(s, se->s)) {
            /* Return a pointer to the symbol table entry. */
            return se;
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
    SymbolTableEntry ste;
    if (st == NULL) return;
    for (i=0;i<st->nBuckets;i++) {
        for (ste = st->tbl[i]; ste; ste=ste->next) {
            for (j=0; j < level; j++) printf("  ");
            printf("%s\n", ste->s);

     /* if this symbol has a subscope, print it recursively, indented
     printsymbols( // subscope symbol table
                    , level+1);
          */
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
int scope_level(SymbolTable *st) {
    return st->level;
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

// Populate symbol tables from AST
void populate_symboltables(struct tree *t, SymbolTable *st) {
    if (t == NULL) {
        return;
    }
    switch (t->node_type) {
        case NODE_ASSIGN:
            insertsymbol(st, t->left->str_val, NULL);
            break;
        case NODE_FUNCTION:
            insertsymbol(st, t->left->str_val, NULL);
            scope_enter(&st);
            populate_symboltables(t->right->left, st);
            scope_exit(&st);
            break;
        case NODE_BLOCK:
            scope_enter(&st);
            populate_symboltables(t->left, st);
            scope_exit(&st);
            break;
        case NODE_IF:
            populate_symboltables(t->left, st);
            scope_enter(&st);
            populate_symboltables(t->right->left, st);
            scope_exit(&st);
            if (t->right->right != NULL) {
                scope_enter(&st);
                populate_symboltables(t->right->right, st);
                scope_exit(&st);
            }
            break;
        case NODE_WHILE:
            populate_symboltables(t->left, st);
            scope_enter(&st);
            populate_symboltables(t->right, st);
            scope_exit(&st);
            break;
        default:
            break;
    }
    populate_symboltables(t->left, st);
    populate_symboltables(t->right, st);
}
```


*/
