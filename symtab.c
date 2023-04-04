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
    if (t == NULL || st == NULL) {
        return;
    }
    // For functions, we add the name of the function to the current symbol 
    // table, then create a symbol table for the function.
    if(strcmp(t->symbolname, "funcdef") == 0) {
        insertfunction(t, st);
        //add_func_type(t, st);
        return;
    }
    if(strcmp(t->symbolname, "classdef") == 0) {
        insertclass(t, st);
        return;
    } 
    if(strcmp(t->symbolname, "global_stmt") == 0) {
        add_global_names(st, t);
        return;
    } 
    if(strcmp(t->symbolname, "expr_stmt") == 0) {
        handle_expr_stmt(t, st);
        return;
    } 
    if(strcmp(t->symbolname, "dotted_as_names") == 0) {
        // Import statements
        get_import_symbols(t, st);
        return;
    } 
    if(strcmp(t->symbolname, "for_stmt") == 0) {
        get_for_iterator(t, st);
        return;
    }
    if(strcmp(t->symbolname, "decl_stmt") == 0) {
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
    if(t == NULL || st == NULL) return;
    if(t->leaf == NULL) return;
    SymbolTableEntry entry = lookup(t->leaf->text, st);
    if(entry == NULL) {
        semantic_error(t->leaf->filename, t->leaf->lineno, "Name '%s' not defined\n", t->leaf->text);
    }
}


/**
 * Get the global symtab;
 */
void add_global_names(SymbolTable st, tree_t *t)
{
    SymbolTable global = get_global_symtab(st);
    if(t->nkids == 0) {
        return;
    }
    if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
        insertsymbol(global, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno);
        add_global_names(global, t->kids[1]);
    }
    else {
        insertsymbol(global, t->kids[1]->leaf->text, t->kids[1]->leaf->lineno);
        add_global_names(global, t->kids[0]);
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

    // Get the name of function in the first child leaf, then add it to the symbol table.
    // If the symboltable already contains the name in either the 
    char *name = t->kids[0]->leaf->text; 
    SymbolTableEntry entry = insertsymbol(st, name, t->kids[0]->leaf->lineno);
    
    // In case a function was already defined, free its symbol table and 
    // make a new one
    free_symtab(entry->nested); 
    entry->nested = mknested(t, HASH_TABLE_SIZE, st, "function");

    // The parent of the function's scope will be the current scope
    entry->nested->parent = st;

    // We will also annotate the tree node with this scope
    t->stab = entry->nested;    // Assign function's scope to the current tree node
    get_function_params(t->kids[1], entry->nested); // Add parameters to function scope
    populate_symboltables(t->kids[3], entry->nested); // Add suite to function scope
}


/**
 * Starting from the parameters rule, navigate to fpdef_equal_test_comma_rep,
 * then recurse the descendants. 
 * fpdef has two children
 */
void get_function_params(struct tree *t, SymbolTable ftable)
{
    if(t == NULL || ftable == NULL)
        return;
    
    // This is the default base type
    int basetype = ANY_TYPE;

    // The fpdef nonterminal contains quite a bit of information about the parameter 
    // and its type, including its name
    if(strcmp(t->symbolname, "fpdef") == 0) {
        if(strcmp(t->kids[1]->symbolname, "colon_test_opt") == 0) {
            // The function param has a type hint
            basetype = get_fpdef_type(t->kids[1], ftable);
        }
        insertsymbol(ftable, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno);
    } else if(strcmp(t->symbolname, "") == 0) {

    }
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], ftable);
        }
    }
}

/**
 * Get function type information
 */
void add_func_type(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    
    // Function name
    char *name = t->kids[0]->leaf->text;

    // FFFFFFFFFF
    SymbolTableEntry entry = lookup_current(name, st);
    
}

/**
 * DEPRECATED
 * For handling assignment semantics
 * Starting subtree is expr_stmt
 * TODO: 
 *  1. Check whether the symbol was previously declared
 *  2. If it was and it's not type ANY, check that the type of RHS is valid
 *  3. Otherwise, assign type ANY
 */
void handle_expr_stmt_depr(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    int basetype;
    // 
    if(t->nkids > 1 && strcmp(t->kids[1]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0)   {
        /* Get the rightmost RHS type of the assignment statement.
        *  Once we have the basetype, we have to verify that all left-hand 
        *  operands are valid */
        //printf("t->kids[1]: %s\n", t->kids[1]->kids[1]->kids[0]->symbolname);
        basetype = get_rhs(t->kids[1]->kids[1], st);
        assign_lhs(basetype, t, st); // Pass the expr_stmt and basetype
        return;
    }
    else if(strcmp(t->symbolname, "trailer") == 0) {
        /* TODO: Handle function/method calls, list subscripting, and object member
        * accesses */
        return ;
    } 
    else if(strcmp(t->symbolname, "expr_conjunct") == 0) {
        /** TODO: Handle stuff like +=, *=, etc. */
        return;
    }
    else if(strcmp(t->symbolname, "power") == 0) {
        if(t->kids[0]->leaf != NULL && strcmp(t->kids[0]->symbolname, "NAME") == 0
        && (t->nkids == 1 || strcmp(t->kids[1]->symbolname, "trailer_rep") != 0)) {
            // If the first child of power has a leaf, and it is a NAME, and either 
            // the current tree has 1 child or its second child is not trailer_rep
            struct token *tok = t->kids[0]->leaf;
            char *ident = tok->text;
        }
    }
    else {
        for(int i = 0; i < t->nkids; i++) {
            if(strcmp(t->kids[i]->symbolname, "equal_OR_yield_OR_testlist_rep") != 0) {
                handle_expr_stmt(t->kids[i], st);
                //rhs = get_assignment_rhs(t->kids[i], st);
            }
        }
    }
}


/**
 * Inside of expr_stmt
 */
void handle_expr_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    // We need check that the current tree has more than one child, and it is an 
    // EYTR
    if(t->nkids > 1 && strcmp(t->kids[1]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0)
    {
        int basetype = get_rhs(t->kids[1]->kids[1], st);
    }
}


/**
 * Get the rightmost right-hand side of an assignment expression. It should 
 * return a basetype integer code. Our assumed starting point is
 * testlist. If multiple consecutive assignments are found we need to verify 
 * that these are also assigned correct types
 */
int get_rhs(struct tree *t, SymbolTable st)
{
    if(strcmp(t->symbolname, "power") == 0 && t->kids[0]->leaf != NULL) {
        /* 'power' can only be reached if we haven't already recursed to a 
         * listmaker or a dictorset_option_* tree node */ 
        struct token *leaf = t->kids[0]->leaf;
        if(leaf->category == NAME) {
            SymbolTableEntry entry = lookup(leaf->text, st);
            if(entry == NULL) {
                semantic_error(leaf->filename, leaf->lineno, "Name '%s' is not defined\n", leaf->text);
            }
            return entry->typ->basetype;
        }
        return get_token_type_code(leaf);
    } 
    else if(strcmp(t->symbolname, "listmaker_opt") == 0) {
        /* Indicates that the right-hand side is a list */
        return LIST_TYPE;
    }
    else if(strcmp(t->symbolname, "dictorsetmaker_opt") == 0) {
        /* Right-hand side is a dictionary */
        return DICT_TYPE;
    }
    else {
        /* It is assumed that we can just recurse the first child until one of 
         * the above three options is found 
         * TODO: Fix bad assumption */
        //printf("recursing: %s\n", t->symbolname);
        return get_rhs(t->kids[0], st);
    }
}

/**
 * Add any valid assignments to the symbol table if they 
 * aren't already present. If the types of LHS and RHS
 * are not compatible, fail. Similarly, if any LHS tokens are not identifiers, 
 * fail. 
 */
void assign_lhs(int basetype, struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return;
    struct token *leaf = NULL;
    if(strcmp(t->symbolname, "expr_stmt") == 0) {
        leaf = get_leftmost_token(t->kids[0], st); // Get the first token in LHS
        // Once the leftmost token is obtained, only recurse further if EYTR 
        // is a child of EYTR
        if(t->kids[1]->kids[0] != NULL) {
            if(strcmp(t->kids[1]->kids[0]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
                assign_lhs(basetype, t->kids[1]->kids[0], st); // expr_conjunct | equal_OR_...
            }
        }
        return;
    } else if(strcmp(t->symbolname, "power") == 0) {
        if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
            leaf = t->kids[0]->leaf;
            // Check if the name is in the symbol table
        }
        return;
    } else if(strcmp(t->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        leaf = get_leftmost_token(t->kids[1], st);
    }
    if(leaf != NULL) {
        SymbolTableEntry entry = lookup(leaf->text, st);
        if(entry == NULL) {
            entry = insertsymbol(st, leaf->text, leaf->lineno);
        } 
        else {
            if(entry->typ->basetype != ANY_TYPE && entry->typ->basetype != basetype) {
                // If the existing entry is not type ANY and it's base type does not 
                // match the basetype of the RHS
                semantic_error(leaf->filename, leaf->lineno,
                    "Type '%s' cannot be assigned to variable of type '%s'\n",
                    get_basetype(basetype), 
                    get_basetype(entry->typ->basetype)
                );
            }
        }
        return;
    }
    for(int i = 0; i < t->nkids; i++) {
        assign_lhs(basetype, t->kids[i], st);
    }
}


/** 
 * Get leftmost token in LHS of assignment
 */
struct token *get_leftmost_token(struct tree *t, SymbolTable st)
{
    struct token *tok = NULL;
    if(t == NULL || st == NULL) 
        return tok;
    if(strcmp(t->symbolname, "power") == 0) {
        // Expect that the first power in the leftmost assignment is a NAME.
        // If it isn't a NAME or it's NULL, throw a syntax error
        if(strcmp(t->kids[0]->symbolname, "atom") == 0) {
            // This happens if the left-hand side contains a list or dict or something
            fprintf(stderr, "LHS of assignment must be a name\n");
            exit(SEM_ERR);
        }
        tok = t->kids[0]->leaf;
        if(tok->category != NAME) {
            // If we find a token on the LHS that isn't a name, e.g., 4 = a = 3
            semantic_error(tok->filename, tok->lineno, "Cannot assign to a literal\n");
        }
        return tok;
    }
    else {
        return get_leftmost_token(t->kids[0], st);
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
    insertsymbol(st, var->text, var->lineno);
}

/**
 * Some boilerplate for searching the symbol tables for valid type hints,
 * then returning the corresponding type int
 */
int determine_hint_type(struct token *type, SymbolTable st)
{
    int basetype = ANY_TYPE;
    SymbolTableEntry type_entry = lookup(type->text, st);
    // If the type entry cannot be found in the symbol table, that's an error
    
    if(type_entry == NULL) 
        semantic_error(type->filename, type->lineno, "Name '%s' is not defined for the provided type\n");
    else {
        // When we find the type entry, we have to consider its type code. If it's 
        // a class, we obtain the class define code if it's a builtin, or ANY_TYPE 
        // otherwise.
        if(type_entry->typ->basetype == CLASS_TYPE) {
            basetype = get_builtins_type_code(type_entry);
        } else {
            // If it's not a class type then let it inherit the type of the RHS
            basetype = type_entry->typ->basetype;
        }
    }
    return basetype;
}

/** 
 * Assign types to the tree nodes
 */
void assigntype(struct tree *t, struct typeinfo *typ)
{

}

/**
 * Get the function param type hint
 */
int get_fpdef_type(struct tree *t, SymbolTable ftable)
{
    if(t == NULL || ftable == NULL) // This probably should never happen
        return ANY_TYPE;
    if(strcmp(t->symbolname, "power") == 0) {
        return determine_hint_type(t->kids[0]->leaf, ftable);
    } else {
        return get_fpdef_type(t->kids[0], ftable);
    }
}

/** 
 * Semantic error printing. Variadic to supply variables
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
        insertsymbol(st, alias->text, alias->lineno);

    } else {
        insertsymbol(st, import_name, dotted_as_name->kids[0]->leaf->lineno);
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
        insertsymbol(st, t->kids[0]->leaf->text, t->kids[0]->leaf->lineno);
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
    SymbolTableEntry entry = insertsymbol(st, name, t->kids[0]->leaf->lineno);
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
SymbolTableEntry insertsymbol(SymbolTable st, char *name, int lineno) {
    if(st == NULL)
        return 0;
        
    // Get the hash index of the given name then find it in the current scope if it exists.
    // If we find the symbol in this loop, we just return its entry. 
    int idx = hash(st, name);
    SymbolTableEntry prev = NULL;
    for (SymbolTableEntry e = st->tbl[idx]; e != NULL; prev = e, e = e->next) {
        if (strcmp(e->ident, name) == 0) {
            return e;
        }
    }
    
    // If we reach this point, we didn't find the symbol in the current scope.
    // Create a symbol table entry, 
    SymbolTableEntry entry = ckalloc(1, sizeof(struct sym_entry));
    entry->typ = ckalloc(1, sizeof(struct typeinfo));
    entry->table = st;
    entry->ident = strdup(name);
    entry->lineno = lineno;
    entry->next = NULL;
    
    // If prev is not NULL we had a collision in the symbol, so we link the list
    if(prev != NULL)
        prev->next = entry;
    else
        st->tbl[idx] = entry;
    //printf("%s\n", entry->ident);
    st->nEntries++;
    return entry;
}


SymbolTableEntry insertbuiltin(SymbolTable st, char *name, int lineno, int basetype)
{
    if(st == NULL)
        return 0;
    SymbolTableEntry entry = insertsymbol(st, name, lineno);
    entry->typ->basetype = basetype;
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
            printf("%s type\n", get_basetype(entry->typ->basetype)); // Switch statements for base types
            if(entry->nested != NULL) {
                printsymbols(entry->nested, level + 1);
            }
        }
    }
}

const char *get_basetype(int basetype)
{
    switch(basetype) {
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
            return "mystery";
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
int get_builtins_type_code(SymbolTableEntry entry)
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

/**
 * Get the base type code from the token 
 * integer codes we used in lexical analysis
 */
int get_token_type_code(struct token *tok)
{
    switch(tok->category) {
        case INTLIT:
            return INT_TYPE;
        case FLOATLIT:
            return FLOAT_TYPE;
        case STRINGLIT:
            return STRING_TYPE;
        case NONE:
            return NONE_TYPE;
        case PYTRUE:
        case PYFALSE:
            return BOOL_TYPE;
        default:
            return ANY_TYPE;
    }
}

void add_puny_builtins(SymbolTable st) {
    insertbuiltin(st, "print", -1, FUNC_TYPE);
    insertbuiltin(st, "int", -1, CLASS_TYPE);
    insertbuiltin(st, "abs", -1, FUNC_TYPE);
    insertbuiltin(st, "bool", -1, CLASS_TYPE);  
    insertbuiltin(st, "chr", -1, FUNC_TYPE);
    insertbuiltin(st, "dict", -1, CLASS_TYPE);
    insertbuiltin(st, "float", -1, CLASS_TYPE);
    insertbuiltin(st, "input", -1, FUNC_TYPE);
    insertbuiltin(st, "int", -1, CLASS_TYPE);    
    insertbuiltin(st, "len", -1, FUNC_TYPE);
    insertbuiltin(st, "list", -1, CLASS_TYPE);
    insertbuiltin(st, "max", -1, FUNC_TYPE);
    insertbuiltin(st, "min", -1, FUNC_TYPE);
    insertbuiltin(st, "open", -1, FUNC_TYPE);
    insertbuiltin(st, "ord", -1, FUNC_TYPE);
    insertbuiltin(st, "pow", -1, FUNC_TYPE);
    insertbuiltin(st, "range", -1, CLASS_TYPE);
    insertbuiltin(st, "round", -1, FUNC_TYPE);
    insertbuiltin(st, "str", -1, CLASS_TYPE);
    insertbuiltin(st, "type", -1, CLASS_TYPE);
}

