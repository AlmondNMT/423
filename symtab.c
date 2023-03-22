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
    printf("t->kids[0]->symbolname: %s\n", t->kids[0]->symbolname);
    if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text);
        add_global_names(st, t->kids[1]);
    }
    else {
        insertsymbol(st, t->kids[1]->leaf->text);
        add_global_names(st, t->kids[0]);
    }
}

/**
 * Insertfunction: Unlike insertsymbol, overwrite any previous definitions of 
 * s in the table. This means free the nested symbol table. An auxiliary function 
 * will recursively descend through the tree to collect identifiers for its 
 * local scope.
 * Assumptions: The first child should contain the function name
 */
void insertfunction(struct tree *t, SymbolTable st)
{
    if(st == NULL)
        return;
    char *name = t->kids[0]->leaf->text;
    int idx = hash(st, name);
    SymbolTableEntry e = NULL, entry = NULL, prev = NULL;
    for(e = st->tbl[idx]; e != NULL; prev = e, e = e->next) {
        if(strcmp(e->s, name) == 0) {
            // If new function definition found, overwrite previous nested table
            entry = e;
            free_symtab(entry->nested);
            entry->nested = mksymtab(HASH_TABLE_SIZE, name);
            populate_symboltables(t->kids[1], entry->nested); // Add parameters 
            populate_symboltables(t->kids[2], entry->nested); // Add rarrow test
            populate_symboltables(t->kids[3], entry->nested); // Add suite
            return;
        }
    }
    entry = calloc(1, sizeof(SymbolTableEntry));
    entry->nested = mksymtab(HASH_TABLE_SIZE, name);

    if(prev != NULL) {
        prev->next = entry;
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
SymbolTable mksymtab(int nbuckets, char *table_name)
{
    SymbolTable rv;
    rv = calloc(1, sizeof(struct sym_table));
    if(rv == NULL)
        return NULL;
    rv->tbl = calloc(nbuckets, sizeof(struct sym_entry *));
    rv->nBuckets = nbuckets;
    rv->nEntries = 0;
    rv->parent = NULL;
    rv->table_name = strdup(table_name);
    return rv;
}

/**
 * Accept the current symbol table and the name of the symbol.
 * Create SymbolTableEntry for the NAME. Add the current table to the entry.
 * Set the relevant fields (s, next, st->tbl[idx])
 */
int insertsymbol(SymbolTable st, char *s) {
    if(st == NULL)
        return 0;
    int idx = hash(st, s);
    for (SymbolTableEntry e = st->tbl[idx]; e != NULL; e = e->next) {
        if (strcmp(e->s, s) == 0) {
            return 0;
        }
    }
    SymbolTableEntry entry = calloc(1, sizeof(struct sym_entry));
    entry->table = st;
    entry->s = strdup(s);
    entry->next = NULL;
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

void printsymbols(SymbolTable st, int level)
{
    int i;
    if (st == NULL) return;
    for (i = 0; i < st->nBuckets; i++) {
        for(SymbolTableEntry entry = st->tbl[i]; entry != NULL; entry = entry->next) {
            printf("Symname: %s\n", entry->s);
        }
    }
}

void free_symtab(SymbolTable st) {
    if (st == NULL) {
        return;
    }

    // Free symbol entries
    for (int i = 0; i < st->nBuckets; i++) {
        SymbolTableEntry entry = st->tbl[i];
        while (entry != NULL) {
            SymbolTableEntry next_entry = entry->next;

            // Free nested symbol table
            if (entry->nested != NULL) {
                free_symtab(entry->nested);
            }

            // Free entry attributes
            if (entry->s != NULL) {
                free(entry->s);
            }
            if (entry->scope != NULL) {
                free(entry->scope);
            }

            // Free entry
            free(entry);

            entry = next_entry;
        }
    }

    // Free referenced but not declared variables
    SymbolTableEntry ref_entry = st->references;
    while (ref_entry != NULL) {
        SymbolTableEntry next_ref_entry = ref_entry->next;

        // Free entry attributes
        if (ref_entry->s != NULL) {
            free(ref_entry->s);
        }
        if (ref_entry->scope != NULL) {
            free(ref_entry->scope);
        }

        // Free entry
        free(ref_entry);

        ref_entry = next_ref_entry;
    }

    // Free the table array and table attributes
    if (st->tbl != NULL) {
        free(st->tbl);
    }
    if (st->table_name != NULL) {
        free(st->table_name);
    }

    // Free the symbol table itself
    free(st);
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

