#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "errdef.h"
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"
#include "utils.h"

extern tree_t* tree;

// Populate symbol tables from AST
void populate_symboltables(struct tree *t, SymbolTable st) {
    if (t == NULL) {
        return;
    }
    // For functions, we add the name of the function to the current symbol 
    // table, then create a symbol table for the function.
    if(strcmp(t->symbolname, "funcdef") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, FUNC_TYPE);
        insertfunction(t, st);
        return;
    } else if(strcmp(t->symbolname, "classdef") == 0) {
        insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, CLASS_TYPE);
        insertclass(t, st);
        return;
    } else if(strcmp(t->symbolname, "global_stmt") == 0) {
        SymbolTable global = get_global_symtab(st);
        add_global_names(global, t);
    } else if(strcmp(t->symbolname, "expr_stmt") == 0) {
        get_assignment_symbols(t, st);
        //insertsymbol(st, t->leaf->text, t->leaf->lineno);
    } else if(strcmp(t->symbolname, "dotted_as_names") == 0) {
        // Import statements
        get_import_symbols(t, st);
        return;
    } else if(strcmp(t->symbolname, "for_stmt") == 0) {
        get_for_iterator(t, st);
        return;
    } else if(strcmp(t->symbolname, "decl_stmt") == 0) {
        get_decl_stmt(t, st);
        return;
    }
    for(int i = 0; i < t->nkids; i++) {
        populate_symboltables(t->kids[i], st);
    }
}


void semantics(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    add_puny_builtins(st);
    populate_symboltables(t, st);
    locate_undeclared(t, st);
}

/**
 * Assumption: symbol table is populated
 */
void locate_undeclared(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    if(strcmp(t->symbolname, "dotted_as_name") == 0) {
        // Don't check names in import stmts
        return;
    }
    else if(strcmp(t->symbolname, "NAME") == 0) {
        //printf("%s\n", t->leaf->text);
        check_decls(t, st);
    }
    for(int i = 0; i < t->nkids; i++) {
        if(t->stab != NULL) 
            locate_undeclared(t->kids[i], t->stab);
        else
            locate_undeclared(t->kids[i], st);
    }
}


/**
 * Assumption: symbol is NAME
 */
void check_decls(struct tree *t, SymbolTable st)
{
    struct token *leaf = t->leaf;
    char *ident = t->leaf->text;
    if(st == NULL) { // It couldn't be found (global->parent = NULL)
        semantic_error(leaf->filename, leaf->lineno, "Name '%s' not defined\n", ident);
        exit(SEM_ERR);
    }
    uint h = hash(st, ident);
    //printf("h: %d\n", h);
    SymbolTableEntry e = NULL;
    for(e = st->tbl[h]; e != NULL; e = e->next) {
        if(strcmp(e->ident, ident) == 0) {
            // Found identifier in hash table
            if(e->lineno <= t->leaf->lineno) {
                //printf("e->llineno: %d\t t lineno: %d\n", e->lineno, t->leaf->lineno);
                return;
            }
        }
    }
    if(e == NULL) {
        check_decls(t, st->parent);
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
        insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, ANY_TYPE);
        add_global_names(st, t->kids[1]);
    }
    else {
        insertsymbol(st, t->kids[1]->leaf->text, t->kids[1]->leaf->lineno, ANY_TYPE);
        add_global_names(st, t->kids[0]);
    }
}

/**
 * Insertfunction: Unlike insertsymbol, overwrite any previous definitions of 
 * s in the table. This means free the nested symbol table. populate_symboltables
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
        if(strcmp(e->ident, name) == 0) {
            // If new function definition found, overwrite previous nested table
            entry = e;
            free_symtab(entry->nested);
            entry->nested = mknested(t, HASH_TABLE_SIZE, st, "function");
            t->stab = entry->nested;
            get_function_params(t->kids[1], entry->nested); // Add parameters 
            //populate_symboltables(t->kids[2], entry->nested); // Add rarrow test
            populate_symboltables(t->kids[3], entry->nested); // Add suite
            return;
        }
    }
    entry = calloc(1, sizeof(SymbolTableEntry));
    entry->nested = mknested(t, HASH_TABLE_SIZE, st, "function"); // make symbol table for function scope
    t->stab = entry->nested;
    populate_symboltables(t->kids[1], entry->nested); // Add parameters 
    //populate_symboltables(t->kids[2], entry->nested); // Add rarrow test
    populate_symboltables(t->kids[3], entry->nested); // Add suite

    if(prev != NULL) {
        prev->next = entry;
    }
}

/**
 * Starting from the parameters rule, navigate to fpdef_equal_test_comma_rep,
 * then recurse through the 
 */
void get_function_params(struct tree *t, SymbolTable ftable)
{
    if(t == NULL || ftable == NULL)
        return;
    if(strcmp(t->symbolname, "fpdef") == 0) {
        if(t->kids[0]->leaf != NULL) 
            insertsymbol(ftable, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, ANY_TYPE);
    }
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], ftable);
        }
    }
}

/**
 * For handling assignment semantics
 * Starting subtree is expr_stmt
 * TODO: Change ANY_TYPE to type of RHS
 */
void get_assignment_symbols(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    if(strcmp(t->symbolname, "power") == 0) {
        if(t->kids[0]->leaf != NULL && strcmp(t->kids[0]->symbolname, "NAME") == 0
                && (t->nkids == 1 || strcmp(t->kids[1]->symbolname, "trailer_rep") != 0)) {
            // If the first child of power has a leaf, and it is a NAME, and either 
            // the current tree has 1 child or its second child is not trailer_rep
            insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, ANY_TYPE);
        }
    }
    else {
        for(int i = 0; i < t->nkids; i++) {
            //if(strcmp(t->kids[i]->symbolname, "equal_OR_yield_OR_testlist_rep") != 0) {
                get_assignment_symbols(t->kids[i], st);
                return;
            //}
        }
    }
}


/** 
 * Add declarations to symbol table
 * TODO: get type info
 */
void get_decl_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    struct token *leaf = t->kids[0]->leaf;
    char *name = leaf->text;
    if(scope_lookup_current(name, st)) {
        // Redeclaration error
        semantic_error(leaf->filename, leaf->lineno, "Redeclaration for name '%s' in scope '%s'\n", name, st->scope);
    }
    insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, ANY_TYPE);
}

/** 
 * Assign types to the tree nodes
 */
void assigntype(struct tree *t, struct typeinfo *typ)
{

}

/** 
 * Semantic error printing
 */
void semantic_error(char *filename, int lineno, char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    int counter = 0;
    fprintf(stderr, "%s:%d: ", filename, lineno);
    while (*msg != '\0') {
        switch (*msg) {
            case '%':
                switch(*(++msg)) {
                    case 'd': {
                        int arg = va_arg(args, int);
                        fprintf(stderr, "%d", arg);
                        break;
                    }
                    case 'f': {
                        double arg = va_arg(args, double);
                        fprintf(stderr, "%f", arg);
                        break;
                    }
                    case 's': {
                        char *arg = va_arg(args, char*);
                        fprintf(stderr, "%s", arg);
                        break;
                    }
                }
                break;
            default:
                fprintf(stderr, "%c", *msg);
        }
        ++msg;
        ++counter;
    }

    va_end(args);

    exit(SEM_ERR);
}

/**
 * TODO: Not sure how to handle attribute accesses yet
 * Search for packages (other python files) in current directory, then 
 * build AST for that package and populate its symbol table
 * Assumption: Current t pointer is pointing at dotted_as_names 
 */
void get_import_symbols(struct tree *t, SymbolTable st)
{
    struct tree *dotted_as_name = t->kids[0];
    struct token *leaf = dotted_as_name->kids[0]->leaf;
    char *import_name = leaf->text;
    char filename[PATHMAX];
    strcpy(filename, import_name);
    strcat(filename, ".py");
    if(access(filename, F_OK) != 0) {
        semantic_error(leaf->filename, leaf->lineno, "No module named '%s'\n", import_name);
    }
    if(strcmp(dotted_as_name->kids[1]->symbolname, "as_name_opt") == 0) { 
        struct token *alias = dotted_as_name->kids[1]->kids[0]->leaf;
        insertsymbol(st, alias->text, alias->lineno, PACKAGE_TYPE);

    } else {
        insertsymbol(st, import_name, dotted_as_name->kids[0]->leaf->lineno, PACKAGE_TYPE);
    }
}

/**
 * Get the iterating variable in for loops. We're only considering loops of the
 * form:
 * ```
 * for var in range(...):
 *   # do something
 * ```
 * Therefore, always INT_TYPE
 * Assume: Starting node is for_stmt
 */
void get_for_iterator(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    if(strcmp(t->symbolname, "power") != 0) {
        get_for_iterator(t->kids[0], st);
        return;
    } else {
        insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno, INT_TYPE);
    }
}

void insertclass(struct tree *t, SymbolTable st)
{
    if(st == NULL)
        return;
    char *name = t->kids[0]->leaf->text;
    int idx = hash(st, name);
    SymbolTableEntry e = NULL, entry = NULL, prev = NULL;
    for(e = st->tbl[idx]; e != NULL; prev = e, e = e->next) {
        if(strcmp(e->ident, name) == 0) {
            // If new class definition found, overwrite previous nested table
            entry = e;
            free_symtab(entry->nested);
            entry->nested = mknested(t, HASH_TABLE_SIZE, st, "class");
            t->stab = entry->nested;
            populate_symboltables(t->kids[1], entry->nested); // Add parameters 
            populate_symboltables(t->kids[2], entry->nested); // Add suite
            return;
        }
    }
    entry = calloc(1, sizeof(SymbolTableEntry));
    entry->nested = mknested(t, HASH_TABLE_SIZE, st, "class"); // make symbol table for function scope
    t->stab = entry->nested;
    populate_symboltables(t->kids[1], entry->nested); // Add parameters 
    populate_symboltables(t->kids[2], entry->nested); // Add suite

    if(prev != NULL) {
        prev->next = entry; // Connect linked list in case of collision
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


uint hash(SymbolTable st, char *s) {
    register uint h = 5381;
    register char c;
    while((c = *s++)) {
        h = (((h << 5) + h) ^ h) + c;
    }
    return h % st->nBuckets;
}

// Create a new symbol table
SymbolTable mksymtab(int nbuckets, char *scope)
{
    SymbolTable rv;
    rv = calloc(1, sizeof(struct sym_table));
    if(rv == NULL)
        return NULL;
    rv->tbl = calloc(nbuckets, sizeof(struct sym_entry *));
    rv->nBuckets = nbuckets;
    rv->nEntries = 0;
    rv->parent = NULL;
    rv->scope = strdup(scope);
    return rv;
}


// Create a symbol table for functions/classes
SymbolTable mknested(struct tree *t, int nbuckets, SymbolTable parent, char *scope)
{
    if(strcmp(parent->scope, "function") == 0) {
        semantic_error(t->leaf->filename, t->leaf->lineno, "Function nesting not allowed in puny\n");
    } else if (strcmp(parent->scope, "class") == 0 && strcmp(scope, "class") == 0) {
        semantic_error(t->leaf->filename, t->leaf->lineno, "Class nesting not allowed in puny\n");
    }
    SymbolTable ftable = mksymtab(nbuckets, scope);
    ftable->level = parent->level + 1;
    ftable->parent = parent;
    return ftable;
}

/**
 * Accept the current symbol table and the name of the symbol.
 * Create SymbolTableEntry for the NAME. Add the current table to the entry.
 * Set the relevant fields (s, next, st->tbl[idx])
 */
int insertsymbol(SymbolTable st, char *s, int lineno, int basetype) {
    if(st == NULL)
        return 0;
    int idx = hash(st, s);
    SymbolTableEntry prev = NULL;
    for (SymbolTableEntry e = st->tbl[idx]; e != NULL; prev = e, e = e->next) {
        if (strcmp(e->ident, s) == 0) {
            return 0;
        }
    }
    SymbolTableEntry entry = calloc(1, sizeof(struct sym_entry));
    entry->typ = calloc(1, sizeof(struct typeinfo));
    entry->typ->basetype = basetype;
    entry->table = st;
    entry->ident = strdup(s);
    entry->lineno = lineno;
    entry->next = NULL;
    if(prev != NULL)
        prev->next = entry;
    else
        st->tbl[idx] = entry;
    //printf("%s\n", entry->ident);
    st->nEntries++;
    return 1;
}


// Helper function to find a symbol in a symbol table
SymbolTableEntry findsymbol(SymbolTable st, char *s)
{
    int h;

    h = hash(st, s);
    for(SymbolTableEntry entry = st->tbl[h]; entry != NULL; entry = entry->next) {
        if(strcmp(s, entry->ident) == 0) {
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
            for(int j = 0; j < entry->table->level; j++) {
                printf("   ");
            }
            printf("%d %s: ", i, entry->ident);
            print_basetype(entry); // Switch statements for base types
            if(entry->nested != NULL) {
                printsymbols(entry->nested, level + 1);
            }
        }
    }
}

void print_basetype(SymbolTableEntry entry)
{
    switch(entry->typ->basetype) {
        case NONE_TYPE:
            printf("None");
            break;
        case INT_TYPE:
            printf("Int");
            break;
        case ANY_TYPE:
            printf("Any");
            break;
        case CLASS_TYPE:
            printf("Class");
            break;
        case LIST_TYPE:
            printf("List");
            break;
        case FLOAT_TYPE:
            printf("Float");
            break;
        case FUNC_TYPE:
            printf("Func");
            break;
        case DICT_TYPE:
            printf("Dict");
            break;
        case BOOL_TYPE:
            printf("Bool");
            break;
        case STRING_TYPE:
            printf("String");
            break;
        case PACKAGE_TYPE:
            printf("Package");
            break;
        default:
            printf("Mystery type");
    }
    printf(" type\n");
}

/** 
 * Liberate the symbol table
 */
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
            if (entry->ident != NULL) {
                free(entry->ident);
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
        if (ref_entry->ident != NULL) {
            free(ref_entry->ident);
        }

        // Free entry
        free(ref_entry);

        ref_entry = next_ref_entry;
    }

    // Free the table array and table attributes
    if (st->tbl != NULL) {
        free(st->tbl);
    }
    if (st->scope != NULL) {
        free(st->scope);
    }

    // Free the symbol table itself
    free(st);
}


int scope_lookup(char *name, SymbolTable st) {
    if(name == NULL)
        return 0;
    SymbolTable curr = st;
    while(curr != NULL) {
        if(scope_lookup_current(name, st)) 
            return 1;
        else
            curr = st -> parent;
    }
    return 0;
}

int scope_lookup_current(char *name, SymbolTable st) {
    if(name == NULL)
        return 0;
    uint h = hash(st, name);
    for(SymbolTableEntry e = st->tbl[h]; e != NULL; e = e->next) {
        if(strcmp(e->ident, name) == 0)
            return 1;
    }
    return 0;
}

void add_puny_builtins(SymbolTable st) {
    insertsymbol(st, "print", -1, FUNC_TYPE);
    insertsymbol(st, "int", -1, CLASS_TYPE);
    insertsymbol(st, "abs", -1, FUNC_TYPE);
    insertsymbol(st, "bool", -1, CLASS_TYPE);  
    insertsymbol(st, "chr", -1, FUNC_TYPE);
    insertsymbol(st, "dict", -1, CLASS_TYPE);
    insertsymbol(st, "float", -1, CLASS_TYPE);
    insertsymbol(st, "input", -1, FUNC_TYPE);
    insertsymbol(st, "int", -1, CLASS_TYPE);    
    insertsymbol(st, "len", -1, FUNC_TYPE);
    insertsymbol(st, "list", -1, CLASS_TYPE);
    insertsymbol(st, "max", -1, FUNC_TYPE);
    insertsymbol(st, "min", -1, FUNC_TYPE);
    insertsymbol(st, "open", -1, FUNC_TYPE);
    insertsymbol(st, "ord", -1, FUNC_TYPE);
    insertsymbol(st, "pow", -1, FUNC_TYPE);
    insertsymbol(st, "range", -1, CLASS_TYPE);
    insertsymbol(st, "round", -1, FUNC_TYPE);
    insertsymbol(st, "str", -1, CLASS_TYPE);
    insertsymbol(st, "type", -1, CLASS_TYPE);
}

