#include <stdarg.h>
#include <stdbool.h>
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
        insertfunction(t, st);
        return;
    } else if(strcmp(t->symbolname, "classdef") == 0) {
        insertclass(t, st);
        return;
    } else if(strcmp(t->symbolname, "global_stmt") == 0) {
        SymbolTable global = get_global_symtab(st);
        add_global_names(global, t);
        return;
    } else if(strcmp(t->symbolname, "expr_stmt") == 0) {
        get_assignment_symbols(t, st);
        return;
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


/**
 * Do semantic analysis here 
 */
void semantics(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    add_puny_builtins(st);          // Add puny builtins like 'int', 'str' and 'open'
    populate_symboltables(t, st);   // Populate symbol table and add type information
    locate_undeclared(t, st);       // Find names that are used, but not declared
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
    if(st == NULL || t == NULL)
        return;
    char *name = t->kids[0]->leaf->text;
    SymbolTableEntry entry = insertsymbol(st, name, t->kids[0]->leaf->lineno, FUNC_TYPE);
    free_symtab(entry->nested); // In case a function was already, overwrite its symboltable
    entry->nested = mknested(t, HASH_TABLE_SIZE, st, "function");
    entry->nested->parent = st;
    t->stab = entry->nested;    // Assign function's scope to the current tree node
    get_function_params(t->kids[1], entry->nested); // Add parameters
    populate_symboltables(t->kids[3], entry->nested); // Add suite to function scope
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
 * TODO: 
 *  1. Check whether the symbol was previously declared
 *  2. If it was and it's not type ANY, check that the type of RHS is valid
 *  3. Otherwise, assign type ANY
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
            struct token *tok = t->kids[0]->leaf;
            char *ident = tok->text;
            SymbolTableEntry entry = lookup(ident, st);
            int basetype = entry->typ->basetype;
            if(entry != NULL && basetype != ANY_TYPE) {
                switch(basetype) {
                    case INT_TYPE:
                        if(tok->category != INTLIT) 
                            semantic_error(tok->filename, tok->lineno, "Wrong type assigned to integer, '%s'\n", tok->text);
                        break;
                    case FLOAT_TYPE:
                        if(tok->category != FLOATLIT)
                            semantic_error(tok->filename, tok->lineno, "Wrong type assigned to float, '%s'\n", tok->text);
                        break;
                    case STRING_TYPE:
                        if(tok->category != STRINGLIT) 
                            semantic_error(tok->filename, tok->lineno, "Wrong type assigned to string, '%s'\n", tok->text);
                        break;
                }
            }
            else insertsymbol(st, tok->text, tok->lineno, ANY_TYPE);
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
 * Gather type info: 
 *  1. Check that the type declaration and optional assignment are consistent
 *  2. Add type information to SymbolTableEntry
 */
void get_decl_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) 
        return;
    struct token *var = t->kids[0]->leaf;
    struct token *type = t->kids[1]->leaf;
    char *name = var->text;
    if(symbol_exists_current(name, st)) {
        // Redeclaration error
        semantic_error(var->filename, var->lineno, "Redeclaration for name '%s' in scope '%s'\n", name, st->scope);
    }
    int basetype = ANY_TYPE;
    SymbolTableEntry type_entry = lookup(type->text, st);
    // If the type entry cannot be found in the symbol table, that's an error
    if(type_entry == NULL)
        semantic_error(type->filename, type->lineno, "Name '%s' is not defined for declaration type\n", type->text);
    else {
        // When we find the type entry, we have to consider its type. If it's
        // a class, we obtain the class define code
        if(type_entry->typ->basetype == CLASS_TYPE) {
            basetype = get_type_code(type_entry);
        }
    }
    insertsymbol(st, var->text, var->lineno, basetype);
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

/**
 * Insertclass: Very similar to insertfunction, except with different parameters 
 * to insertsymbol, mknested, and populate_symboltables
 */
void insertclass(struct tree *t, SymbolTable st)
{
    if(st == NULL || t == NULL)
        return;
    char *name = t->kids[0]->leaf->text;
    SymbolTableEntry entry = insertsymbol(st, name, t->kids[0]->leaf->lineno, CLASS_TYPE);
    free_symtab(entry->nested);
    entry->nested = mknested(t, HASH_TABLE_SIZE, st, "class");
    t->stab = entry->nested;
    populate_symboltables(t->kids[1], entry->nested); // Add class params?? TODO: Check this
    populate_symboltables(t->kids[2], entry->nested); // Add class suite
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
    rv = ckalloc(1, sizeof(struct sym_table));
    rv->tbl = ckalloc(nbuckets, sizeof(struct sym_entry *));
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
SymbolTableEntry insertsymbol(SymbolTable st, char *s, int lineno, int basetype) {
    if(st == NULL)
        return 0;
    int idx = hash(st, s);
    SymbolTableEntry prev = NULL;
    for (SymbolTableEntry e = st->tbl[idx]; e != NULL; prev = e, e = e->next) {
        if (strcmp(e->ident, s) == 0) {
            return e;
        }
    }
    SymbolTableEntry entry = ckalloc(1, sizeof(struct sym_entry));
    entry->typ = ckalloc(1, sizeof(struct typeinfo));
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
    return entry;
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
            printf("%s type\n", get_basetype(entry)); // Switch statements for base types
            if(entry->nested != NULL) {
                printsymbols(entry->nested, level + 1);
            }
        }
    }
}

const char *get_basetype(SymbolTableEntry entry)
{
    switch(entry->typ->basetype) {
        case NONE_TYPE:
            return "None";
        case INT_TYPE:
            return "int";
        case ANY_TYPE:
            return "any";
        case CLASS_TYPE:
            return "class";
        case LIST_TYPE:
            return "list";
        case FLOAT_TYPE:
            return "float";
        case FUNC_TYPE:
            return "func";
        case DICT_TYPE:
            return "dict";
        case BOOL_TYPE:
            return "bool";
        case STRING_TYPE:
            return "str";
        case PACKAGE_TYPE:
            return "package";
        default:
            return "Mystery type";
    }
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


int symbol_exists(char *name, SymbolTable st) {
    return lookup(name, st) != NULL;
}

int symbol_exists_current(char *name, SymbolTable st) {
    return lookup_current(name, st) != NULL;
}

SymbolTableEntry lookup(char *name, SymbolTable st)
{
    if(name == NULL) 
        return NULL;
    SymbolTable curr = st;
    SymbolTableEntry entry = NULL;
    while(curr != NULL) {
        if((entry = lookup_current(name, curr)) != NULL)
            return entry;
        else
            curr = curr->parent;
    }
    return entry;
}

SymbolTableEntry lookup_current(char *name, SymbolTable st)
{
    if(name == NULL) 
        return NULL;
    uint h = hash(st, name);
    SymbolTableEntry e = NULL;
    for(e = st->tbl[h]; e != NULL; e = e->next) {
        if(strcmp(e->ident, name) == 0)
            return e;
    }
    return e;
}

/**
 * This is called if the entry is a CLASS_TYPE, to determine if it is also a 
 * builtin. If it is a builtin, return the integer code, otherwise return ANY_TYPE
 */
int get_type_code(SymbolTableEntry entry)
{
    if(entry == NULL)
        return 0;
    char *ident = entry->ident;
    if(strcmp(ident, "int") == 0)
        return INT_TYPE;
    else if(strcmp(ident, "list") == 0)
        return LIST_TYPE;
    else if(strcmp(ident, "float") == 0)
        return FLOAT_TYPE;
    else if(strcmp(ident, "dict") == 0)
        return DICT_TYPE;
    else if(strcmp(ident, "bool") == 0)
        return BOOL_TYPE;
    else if(strcmp(ident, "str") == 0)
        return STRING_TYPE;
    return ANY_TYPE;
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

