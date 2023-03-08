#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

// Create a new symbol table
SymbolTable mksymtab() {
    SymbolTable st = calloc(1, sizeof(struct sym_table));
    st->nEntries = 0;
    st->next = NULL;
    return st;
}

// Helper function to find a symbol in a symbol table
SymbolTableEntry findsymbol(SymbolTable st, char *s) {
    SymbolTableEntry ste = st->next;
    while(ste != NULL) {
        if(strcmp(ste->s, s) == 0) {
            return ste;
        }
        ste = ste->next;
    }
    return NULL;
}

// Insert a symbol into the symbol table
SymbolTableEntry insertsymbol(SymbolTable st, char *s) {
    SymbolTableEntry ste = findsymbol(st, s);
    if(ste != NULL) {
        // Symbol already exists
        return ste;
    }

    // Symbol does not exist, insert it
    ste = calloc(1, sizeof(struct sym_entry));
    ste->s = s;
    ste->next = st->next;
    st->next = ste;
    st->nEntries++;
    return ste;
}

// Remove a symbol from the symbol table
SymbolTableEntry removesymbol(SymbolTable st, char *s) {
    SymbolTableEntry ste = st->next;
    SymbolTableEntry prev = NULL;
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
    SymbolTableEntry ste = st->next;
    while(ste != NULL) {
        printf("\tSymbol: %s\n", ste->s);
        ste = ste->next;
    }
}