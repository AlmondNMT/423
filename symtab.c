#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"

extern YYSTYPE yylval;

void populate_symboltables(struct tree *t, SymbolTable st)
{
    int i;
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

SymbolTable mksymtab(int nbuckets)
{
    SymbolTable rv;
    rv = calloc(1, sizeof(struct sym_table));
    rv->tbl = calloc(nbuckets, sizeof(struct sym_entry *));
    rv->nBuckets = nbuckets;
    rv->nEntries = 0;
    return rv;
}

/*
 * Insert a symbol into a symbol table.
 */
int insert_sym(SymbolTable st, char *s)
{
    int h;
    SymbolTableEntry se;
    h = hash(st, s);
    for(se = st->tbl[h]; se != NULL; se = se->next) {
        if(!strcmp(s, se->s)) { 
            /* A copy of the string is already in the table. */
            /* TODO: Implement collision handling */
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

SymbolTableEntry lookup_st(SymbolTable st, char *s)
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
*/
