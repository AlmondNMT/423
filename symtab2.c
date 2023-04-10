#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "nonterminal.h"
#include "tree.h"
#include "type.h"
#include "utils.h"
#include "built_in_list.h"

struct typeinfo *get_trailer_type2(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{   

    if(t == NULL || st == NULL ||  entry == NULL)
    {   
        fprintf(stderr, "ERROR get_trailer_type: one or more arguments is null\n");
        exit(SEM_ERR);
    }

    struct typeinfo *type = NULL, *rhs_type = NULL;
    SymbolTableEntry rhs = NULL;
    if(strcmp(t->symbolname, "trailer_rep") == 0) {
        // If "trailer_rep"'s second child is a "trailer", then we need
        //   to get the RHS of trailer
        // If we find a trailer rep, then we know we must go further
        if(strcmp(t->kids[1]->symbolname, "trailer") == 0) {
            
            if(strcmp(t->kids[1]->kids[0]->symbolname, "NAME") == 0) {

            }
        }
        if(strcmp(t->kids[0]->symbolname, "trailer_rep") == 0) {
            type = get_trailer_type2(t->kids[0], st, entry);
        }
        // "trailer_rep's" first child is a "nulltree", but it's 
        //    second child is something to traverse
        else {
            type = get_trailer_type2(t->kids[1], st, entry);
        }
        return type;
    }
}