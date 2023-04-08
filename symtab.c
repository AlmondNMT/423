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
void semantics(struct tree *tree, SymbolTable st)
{
    if(tree == NULL || st == NULL) 
        return;
    
    // We first search for invalid expressions containing either functions or
    //  literals on the LHS of assignments. Python2.7 and Python3 both handle 
    //  invalid expressions before populating ST and type-checking
    locate_invalid_expr(tree);
    
    // Add puny builtins like 'int', 'str' and 'open'
    add_puny_builtins(st);          

    // Populate symbol table and add type information
    populate_symboltables(tree, st);   

    // Find names that are used, but not declared
    locate_undeclared(tree, st);       

    // Ensure that operand types are valid
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
    if(entry == NULL || t->leaf->lineno < entry->lineno) {
        semantic_error(t->leaf->filename, t->leaf->lineno, "Name '%s' not defined\n", t->leaf->text);
    }
}


/**
 * Get the global symtab;
 */
void add_global_names(SymbolTable st, tree_t *t)
{
    // Get the global symbol table
    SymbolTable global = get_global_symtab(st);
    if(t->nkids == 0) {
        return;
    }

    // The leaf 
    struct token *leaf = NULL;

    // The symbol name
    if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
        leaf = t->kids[0]->leaf;
        insertsymbol(global, leaf->text, leaf->lineno, leaf->filename, ANY_TYPE);
        add_global_names(global, t->kids[1]);
    }
    else {
        leaf = t->kids[1]->leaf; 
        insertsymbol(global, leaf->text, leaf->lineno, leaf->filename, ANY_TYPE);
        add_global_names(global, t->kids[0]);
    }
}

/**
 * Insertfunction: Unlike insertsymbol, overwrite any previous definitions of 
 * s in the table. This means free the nested symbol table. populate_symboltables
 * will recursively descend through the tree to collect identifiers for its 
 * local scope.
 * Assumptions: The first child should contain the function name, cuz our 
 *   starting nonterminal is funcdef
 */
void insertfunction(struct tree *t, SymbolTable st)
{
    if(st == NULL || t == NULL)
        return;

    // Get the name of function in the first child leaf, then add it to the symbol table.
    // If the symboltable already contains the name in either the 
    struct token *leaf = t->kids[0]->leaf;
    char *name = leaf->text; 
    SymbolTableEntry entry = insertsymbol(st, name, leaf->lineno, leaf->filename, FUNC_TYPE);
    
    // In case a function was already defined, free its symbol table and 
    // make a new one
    free_symtab(entry->nested); 
    entry->nested = mknested(leaf->filename, leaf->lineno, HASH_TABLE_SIZE, st, "function");

    // The parent of the function's scope will be the current scope
    entry->nested->parent = st;

    // allocate the function typeinfo pointer
    if(entry->typ->u.f.returntype == NULL) {
        entry->typ->u.f.returntype = ckalloc(1, sizeof(struct typeinfo));
        entry->typ->u.f.st = entry->nested;
    }
    
    // We will also annotate the tree node with this scope
    t->stab = entry->nested;    // Assign function's scope to the current tree node
    get_function_params(t->kids[1], entry->nested);   // Add parameters to function scope
    add_func_type(t->kids[2], st, entry);
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
        struct token *leaf = t->kids[0]->leaf;
        insertsymbol(ftable, leaf->text, leaf->lineno, leaf->filename, basetype);
    } 
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], ftable);
        }
    }
}

/**
 * Traverse the rarrow subtree and add the type information to the entry.
 * We are starting at rarrow_opt here
 */
void add_func_type(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{
    if(entry == NULL || st == NULL || t == NULL)
        return;

    // If we reach a power 
    int basetype = get_rhs_type(t, st)->basetype;
    entry->typ->u.f.returntype->basetype = basetype;
}

/**
 * Inside of expr_stmt, a very complex branch of the syntax
 */
void handle_expr_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    
    SymbolTableEntry entry = NULL;
    // At the highest level, there seems to be three kinds of expr_stmts: 
    //    1. Assignments (equal_OR_yield_OR_testlist_rep)
    //    2. Aug-assigns (expr_conjunct)
    //    3. Plain function calls, list accesses, and arithmetic expressions
    // EYTR indicates an assignment. It can be the second child of expr_stmt
    if(strcmp(t->kids[1]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        // Get the type of the rightmost argument by passing 'testlist' node
        struct typeinfo *rhs_type = get_rhs_type(t->kids[1]->kids[1], st);
        int basetype = rhs_type->basetype;

        // Get the leftmost token first due to the shape of the tree
        struct token *leftmost = get_leftmost_token(t, st, basetype);
        printf("%s: %s\n", leftmost->text, get_basetype(basetype));

        // Add the leftmost op to the symbol table if it doesn't already exist, 
        //   then verify type compatibility with the rightmost operand
        entry = insertsymbol(st, leftmost->text, leftmost->lineno, leftmost->filename, ANY_TYPE);
        check_var_type(entry, basetype, leftmost->lineno);

        // If there's any assignment chaining, verify the types of those 
        //   operands, and potentially add them to the table
        handle_eytr_chain(t->kids[1]->kids[0], st, basetype);
    }
}


/**
 * Verify type compatibility between LHS operands and the type of the rightmost
 * operand
 * TODO: Finishing variable type checking
 */
void check_var_type(SymbolTableEntry entry, int rhs_type, int lineno)
{
    if(entry == NULL || entry->typ == NULL) return;

    // If the right-hand-side of an assignment has type any, this is runtime's 
    //   problem
    if(rhs_type == ANY_TYPE) return;

    // If the basetype of the entry is not ANY_TYPE, then check it against
    //   rhs type
    if(entry->typ->basetype != ANY_TYPE) {
        // TODO
        if(entry->typ->basetype != rhs_type) {
            semantic_error(entry->filename, lineno,
                    "incompatible assignment between '%s' and '%s' near operand '%s'\n", 
                    get_basetype(entry->typ->basetype), 
                    get_basetype(rhs_type),
                    entry->ident
                    );
        }
    }

}


/**
 * Auxiliary function for handling assignment chains
 */
void handle_eytr_chain(struct tree *t, SymbolTable st, int basetype)
{
    if(t == NULL || st == NULL) return;
    if(strcmp(t->symbolname, "power") == 0) {
        handle_token(t, st);
        return;
    }
    // For nested EYTR assignment chains
    if(strcmp(t->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        handle_eytr_chain(t->kids[0], st, basetype);
        handle_eytr_chain(t->kids[1], st, basetype);
    }
    for(int i = 0; i < t->nkids; i++) {
        handle_eytr_chain(t->kids[i], st, basetype);
    }
}



/*
 * For handling statements like these: 
 *   "class Obj:
 *        b: int
 *        b = 1
 *    
 *    class Nest:
 *        a: Obj
 *        a = Obj()
 *    c = Nest()
 *    d: int
 *    d = c.a.b
 *   "
 * TODO: 
SymbolTableEntry get_trailer_entry(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return NULL;
    
    // If we find a trailer, we need to 
    if(strcmp(t->symbolname, "trailer") == 0) {

    }
    else if(strcmp(t->symbolname, "trailer_rep") == 0) {
    }
} */

/**
 * This function is called for handling "power" nonterminals in assignments. 
 * For example, when `a` is found in the expression `a = b = c = 1`, this 
 * function should be called to add it to the table. If we find a trailer,
 * which indicates either a function call, a list access, or a member access,
 * then the leftmost token must be in the symboltable
 *
 * ASSUMPTIONS: 
 *   1. Our starting nonterminal is a 'power'
 *   2. We do not expect to see an 'atom' nonterminal as the first child
 */
void handle_token(struct tree *t, SymbolTable st)
{
    // First check that we are currently at a power nonterminal
    if(strcmp(t->symbolname, "power") == 0) {

        // Then check if our first child is a NAME. If it is not, throw a fit,
        //   because this violates the assumption that only NAMES will be found,
        //   and not listmaker_opts, dictorsetmaker_opts, or literals
        if(strcmp(t->kids[0]->symbolname, "NAME") != 0) {
            fprintf(stderr, "LHS of assignment must contain an identifier\n");
            exit(SEM_ERR);
        }
        
        // Grab the leftmost identifier, then look it up in the symbol table
        struct token *left = t->kids[0]->leaf;
        SymbolTableEntry entry = lookup(left->text, st);

        // Now check if "power" has a second child, "trailer_rep". If so, do the 
        //   following: 
        //     1. Try to find the entry in the symbol table; throw an error if 
        //        it's not present
        //     2. Recursively retrieve entries from the symbol tables of each
        //        "trailer_rep" token that is found. This is done with an 
        //        auxiliary function. This should return a single table entry
        //        that can be type-annotated
        if(strcmp(t->kids[1]->symbolname, "trailer_rep") == 0) {

            // If we could not find an entry in the table for the leftmost 
            //   token, throw a fit. For example, "a.b = 2" is only valid if 
            //   "a" is previously defined
            if(entry == NULL) {
                semantic_error(left->filename, left->lineno, "name '%s' is not defined\n", left->text);
            }
            
            // Get the nested symbol table of the entry, then call an auxiliary 
            //   function to locate the member being accessed. 
            printf("entry->ident: %s\n", entry->ident);
            entry = get_chained_dot_entry(t->kids[1], st, entry);
            
        }
        // TODO: If you want chained assignments to work, uncomment this line below
        //entry = insertsymbol(st, left->text, left->lineno, left->filename, ANY_TYPE);
    }
}


/**
 * Auxiliary function for handling dot chains (i.e. "a.b.c")
 * Called by 
 *   get_leftmost_token
 *   handle_token
 *
 * Assumed starting position: "trailer_rep"
 */
SymbolTableEntry get_chained_dot_entry(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{
    if(t == NULL || st == NULL || entry->nested == NULL) return NULL;
    SymbolTableEntry rhs = NULL;

    // Perform DFS on trailer_rep to get the immediate RHS of the current entry
    if(strcmp(t->symbolname, "trailer_rep") == 0) {
        printf("entry->ident: %s\n", entry->ident);
        // DFS
        rhs = get_chained_dot_entry(t->kids[0], st, entry);
        
        // The first out of two possible cases: the child of "trailer" is a 
        //   name in a dot operation. 
        // In this example here, the second child of "trailer_rep",
        //   "t->kids[1]", is a "trailer". The first child of "trailer" is 
        //   either a "NAME" or a "subscriptlist".
        if(strcmp(t->kids[1]->kids[0]->symbolname, "NAME") == 0) {
            struct token *rhs_tok = t->kids[1]->kids[0]->leaf;

            // Get the symbol table of the rhs
            rhs = lookup_current(rhs_tok->text, entry->nested);
            if(rhs == NULL) {
                semantic_error(rhs_tok->filename, rhs_tok->lineno, "Name '%s' does not belong to class: %s\n", rhs_tok->text, get_basetype(entry->typ->basetype));
            }
        }

        // For lists
        else if(strcmp(t->kids[1]->kids[0]->symbolname, "subscriptlist") == 0){
            
        }

        // For function/constructor calls
        else {

        }
    }
    return rhs;
}


/**
 * Search for expressions containing literals or function calls on the LHS of 
 * assignments. This can begin in the topmost node
 */
void locate_invalid_expr(struct tree *t)
{
    if(t == NULL) return;
    if(strcmp(t->symbolname, "expr_stmt") == 0) {
        // Assignments with EQUAL signs are indicated by this nasty EYTR 
        //   nonterminal, which can be the second child of an expr_stmt
        if(strcmp(t->kids[1]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
            // We now search everywhere except the RHS for any literals, lists,
            //  or dicts, or arithmetic expressions
            locate_invalid_leftmost(t);
            locate_invalid_nested(t);
        }
        // Once we find an expr_stmt, we return to avoid traversing its 
        //   subtrees twice for no reason
        return;
    } 
    else {
        for(int i = 0; i < t->nkids; i++) {
            // Recurse all the children
            locate_invalid_expr(t->kids[i]);
        }
    }
}

/**
 * Search for any invalid expressions on the leftmost branch of expr_stmt
 * Assumption: Our starting point is an expr_stmt
 */
void locate_invalid_leftmost(struct tree *t)
{
    if(t == NULL) return;

    // This makes it illegal to have weird arithmetic and logical expressions
    //   to the left of an assignment
    locate_invalid_arith(t);

    // If we encounter power nonterms, we must ensure there are no 
    //   function/constructor to the LHS of assignments
    if(strcmp(t->symbolname, "power") == 0) {
        //   If we find a trailer nonterminal and it doesn't have a child NAME, 
        // throw an error saying 'cannot assign to function call'. This behavior
        // differs from Python's in that any puny function calls on the LHS of  
        // an assignment are erroneous, whereas Python only throws an error if 
        // the rightmost function call in a dotted trailer occurs.
        locate_invalid_trailer(t->kids[1]);

        // Throw a semantic error if a non-NAME token is found in the LHS of an 
        // assignment
        locate_invalid_token(t->kids[0]);
    }
    else {
        // The leftmost branch contains the first token/item in an assignment
        locate_invalid_leftmost(t->kids[0]);
    }
}


/**
 * One child policy for LHS of assignments: no arithmetic on the left side of 
 *   assignments
 */
void locate_invalid_arith(struct tree *t)
{
    if(strcmp(t->symbolname, "or_test") == 0 
            || strcmp(t->symbolname, "and_test") == 0
            || strcmp(t->symbolname, "comparison") == 0
            || strcmp(t->symbolname, "expr") == 0
            || strcmp(t->symbolname, "xor_expr") == 0
            || strcmp(t->symbolname, "and_expr") == 0
            || strcmp(t->symbolname, "shift_expr") == 0
            || strcmp(t->symbolname, "arith_expr") == 0
            || strcmp(t->symbolname, "term") == 0) {
        if(strcmp(t->kids[1]->symbolname, "nulltree") != 0) {
            fprintf(stderr, "cannot assign to operator\n");
            exit(SEM_ERR);
        }
    }
}

/**
 * Assumed starting point is expr_stmt
 */
void locate_invalid_nested(struct tree *t)
{
    if(t == NULL) return;
    // If the first child of EYTR is another EYTR, search for any violations
    if(strcmp(t->kids[1]->kids[0]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        locate_invalid_nested_aux(t->kids[1]->kids[0]);
    }
}

/**
 * This one is called if there are nested EYTR nodes, i.e., chained assignments
 */
void locate_invalid_nested_aux(struct tree *t)
{
    if(t == NULL) return;
    // Ensure there is no illegal arithmetic/logical operators on LHS
    locate_invalid_arith(t);

    // If the first child of power is not an identifier, then check for an invalid trailer
    if(strcmp(t->symbolname, "power") == 0) {
        locate_invalid_trailer(t->kids[1]);
        locate_invalid_token(t->kids[0]);
        return;
    }
    // 
    if(strcmp(t->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        locate_invalid_nested_aux(t->kids[0]);
        locate_invalid_nested_aux(t->kids[1]);
        return;
    }
    for(int i = 0; i < t->nkids; i++) {
        locate_invalid_nested_aux(t->kids[i]);
    }
}

/**
 * Starting point is either a nulltree, a trailer_rep, or a trailer 
 */
void locate_invalid_trailer(struct tree *t)
{
    if(t == NULL) return;
    
    // If we find a trailer, we throw an error if it's first child is not a NAME
    if(strcmp(t->symbolname, "trailer") == 0) {

        // arglist_opt denotes a function call, which is illegal on the LHS of assignments
        if(strcmp(t->kids[0]->symbolname, "arglist_opt") == 0) {
            fprintf(stderr, "Cannot assign to function call\n");
            exit(SEM_ERR);
        }
    }
    // If we're at a trailer_rep, we must search further
    else if(strcmp(t->symbolname, "trailer_rep") == 0) {
        for(int i = 0; i < t->nkids; i++)
            locate_invalid_trailer(t->kids[i]);
    }
}

/**
 * Starting point: power->kids[0]. If the current node is not a name, and 
 */
void locate_invalid_token(struct tree *t)
{
    if(t == NULL) return;
    if(strcmp(t->symbolname, "NAME") != 0 && t->leaf != NULL)
        semantic_error(t->leaf->filename, t->leaf->lineno, "Cannot assign to literal\n");
}   


/**
 * Get the rightmost right-hand side of an assignment expression. It should 
 * return a basetype integer code. Our assumed starting point is
 * testlist. If multiple consecutive assignments are found we need to verify 
 * that these are also assigned correct types
 */
struct typeinfo *get_rhs_type(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return alcbuiltin(ANY_TYPE);

    // Recurse until the "power" nonterminal is found
    if(strcmp(t->symbolname, "power") == 0 && t->kids[0]->leaf != NULL) {
        // 'power' can only be reached if we haven't already recursed to a 
        //   listmaker or a dictorset_option_* tree node 
        // This leaf contains the leftmost name of the expr_stmt
        struct token *leaf = t->kids[0]->leaf;

        // If the RHS token is a name
        if(leaf->category == NAME) {
            SymbolTableEntry entry = lookup(leaf->text, st);
            if(entry == NULL) {
                semantic_error(leaf->filename, leaf->lineno, "Name '%s' is not defined\n", leaf->text);
            }
            return get_builtins_type(entry->ident);
        }

        // If the RHS token is a literal, dict, list, bool, or None
        return get_token_type(leaf);
    } 

    // If we see listmaker_opt, we know that it's a list (e.g., [1, 2, b])
    else if(strcmp(t->symbolname, "listmaker_opt") == 0) {
        printf("asdfasdf\n");
        return alclist();
    }

    // Dictionary
    else if(strcmp(t->symbolname, "dictorsetmaker_opt") == 0) {
        /* Right-hand side is a dictionary */
        return alcdict();
    }
    else {
        /* It is assumed that we can just recurse the first child until one of 
         * the above three options is found 
         * TODO: Fix bad assumption */
        //printf("recursing: %s\n", t->symbolname);
        return get_rhs_type(t->kids[0], st);
    }
}

/** 
 * Get leftmost token in LHS of assignment
 */
struct token *get_leftmost_token(struct tree *t, SymbolTable st, int basetype)
{
    struct token *tok = NULL;
    if(t == NULL || st == NULL) 
        return tok;
    
    if(strcmp(t->symbolname, "power") == 0) {
        // Expect that the first power in the leftmost assignment is a NAME.
        // If it isn't a NAME or it's NULL, throw a syntax error
        
        tok = t->kids[0]->leaf;
        handle_token(t, st);
        return tok;
    }
    else {
        return get_leftmost_token(t->kids[0], st, basetype);
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
    if(t == NULL || st == NULL) return;

    // The 'var' token has the NAME of the identifier
    // The 'type' token has the NAME of the type
    struct token *var = t->kids[0]->leaf;
    struct token *type = t->kids[1]->leaf;
    char *name = var->text;
    if(symbol_exists_current(name, st)) {
        // Redeclaration error
        semantic_error(var->filename, var->lineno, "Redeclaration for name '%s' in scope '%s'\n", name, st->scope);
    }
    int basetype = determine_hint_type(type, st);
    insertsymbol(st, var->text, var->lineno, var->filename, basetype);
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
        semantic_error(type->filename, type->lineno, "Name '%s' is not defined for the provided type\n", type->text);
    else {
        // When we find the type entry, we have to consider its type code. If it's 
        // a class, we obtain the class define code if it's a builtin, or ANY_TYPE 
        // otherwise.
        if(type_entry->typ->basetype == CLASS_TYPE) {
            basetype = get_builtins_type_code(type_entry->ident);

        } else {
            // If it's not a class type then let it inherit the type of the RHS
            basetype = type_entry->typ->basetype;
        }
    }
    return basetype;
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

/*
Checks if given import_name is a built_in liberry
Depends on #include "built_in_list.h"
*/
int is_built_in(char *import_name) {
    
    
    // Create a pointer to iterate over the elements of built_in_list
    char **iterator = built_in_list;
    
    //will iterate until it finds LIST_END in the built in list, if it does return 0
     while (*iterator != NULL) {
        
        //if import_name was found in built in list, return 1
        if (strcmp(import_name, *iterator) == 0)
            return 1;
        
        iterator++;
    }
    
    return 0;
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

    // If the file is not found in the current AND it is not a builtin
    if(access(filename, F_OK) != 0 && !is_built_in(import_name)) {
        semantic_error(leaf->filename, leaf->lineno, "No module named '%s'\n", import_name);
    }
    if(strcmp(dotted_as_name->kids[1]->symbolname, "as_name_opt") == 0) { 
        struct token *alias = dotted_as_name->kids[1]->kids[0]->leaf;
        insertsymbol(st, alias->text, alias->lineno, alias->filename, PACKAGE_TYPE);

    } else {
        struct token *tok = dotted_as_name->kids[0]->leaf;
        insertsymbol(st, import_name, tok->lineno, tok->filename, PACKAGE_TYPE);
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
        struct token *leaf = t->kids[0]->leaf;
        insertsymbol(st, leaf->text, leaf->lineno, leaf->filename, ANY_TYPE);
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
    struct token *leaf = t->kids[0]->leaf;
    char *name = t->kids[0]->leaf->text;
    SymbolTableEntry entry = insertsymbol(st, name, leaf->lineno, leaf->filename, CLASS_TYPE);
    free_symtab(entry->nested);
    entry->nested = mknested(leaf->filename, leaf->lineno, HASH_TABLE_SIZE, st, "class");
    entry->typ = alcclass(name);

    // Point the typeinfo symbol table to the class entry symbol table 
    //   (possibly redundant)
    entry->typ->u.cls.st = entry->nested;

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
SymbolTable mknested(char *filename, int lineno, int nbuckets, SymbolTable parent, char *scope)
{
    if(strcmp(parent->scope, "function") == 0) {
        semantic_error(filename, lineno, "Function nesting not allowed in puny\n");
    } else if (strcmp(parent->scope, "class") == 0 && strcmp(scope, "class") == 0) {
        semantic_error(filename, lineno, "Class nesting not allowed in puny\n");
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
SymbolTableEntry insertsymbol(SymbolTable st, char *name, int lineno, char *filename, int basetype) {
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
    // Create a symbol table entry and add its members
    SymbolTableEntry entry = ckalloc(1, sizeof(struct sym_entry));
    entry->typ = ckalloc(1, sizeof(struct typeinfo));
    entry->typ->basetype = basetype;
    entry->table = st;
    entry->ident = strdup(name);
    entry->lineno = lineno;
    entry->filename = strdup(filename);
    entry->next = NULL;
    
    // If prev is not NULL we had a collision in the table, so we link the 
    //   symbol list 
    if(prev != NULL)
        prev->next = entry;
    else
        st->tbl[idx] = entry;

    // Increase the number of entries in the symbol table
    st->nEntries++;
    return entry;
}


void printsymbols(SymbolTable st)
{
    if (st == NULL) return;

    // Print every symbol in the current table, including all sub-tables
    for (int i = 0; i < st->nBuckets; i++) {
        
        // For every entry in a collision list...
        for(SymbolTableEntry entry = st->tbl[i]; entry != NULL; entry = entry->next) {

            // Print a couple spaces for each level of table nesting
            for(int j = 0; j < entry->table->level; j++) {
                printf("   ");
            }
            
            printf("%d %s: ", i, entry->ident);
            printf("%s type\n", get_basetype(entry->typ->basetype)); // Switch statements for base types
            if(entry->nested != NULL) {
                printsymbols(entry->nested);
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
int get_builtins_type_code(char *ident)
{
    return get_builtins_type(ident)->basetype;
}


struct typeinfo *get_builtins_type(char *ident)
{
    if(strcmp(ident, "int") == 0)
        return alcbuiltin(INT_TYPE);
    else if(strcmp(ident, "list") == 0)
        return alclist();
    else if(strcmp(ident, "float") == 0)
        return alcbuiltin(FLOAT_TYPE);
    else if(strcmp(ident, "dict") == 0)
        return alcdict();
    else if(strcmp(ident, "bool") == 0)
        return alcbuiltin(BOOL_TYPE);
    else if(strcmp(ident, "str") == 0)
        return alcbuiltin(STRING_TYPE);
    return alcbuiltin(ANY_TYPE);
}

/**
 * Get the base type code from the token 
 * integer codes we used in lexical analysis
 */
int get_token_type_code(struct token *tok)
{
   return get_token_type(tok)->basetype;
}

struct typeinfo *get_token_type(struct token *tok)
{
    switch(tok->category) {
        case INTLIT:
            return alcbuiltin(INT_TYPE);
        case FLOATLIT:
            return alcbuiltin(FLOAT_TYPE);
        case STRINGLIT:
            return alcbuiltin(STRING_TYPE);
        case NONE:
            return alcbuiltin(NONE_TYPE);
        case PYTRUE:
        case PYFALSE:
            return alcbuiltin(BOOL_TYPE);
        default:
            return alcbuiltin(ANY_TYPE);
    }
}

void add_puny_builtins(SymbolTable st) {
    SymbolTableEntry entry = NULL;

    insertsymbol(st, "any", -1, "(builtins)", CLASS_TYPE);
    insertsymbol(st, "print", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "int", -1, "(builtins)", CLASS_TYPE);
    insertsymbol(st, "abs", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "bool", -1, "(builtins)", CLASS_TYPE);  
    insertsymbol(st, "chr", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "float", -1, "(builtins)", CLASS_TYPE);
    insertsymbol(st, "input", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "int", -1, "(builtins)", CLASS_TYPE);    
    insertsymbol(st, "len", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "max", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "min", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "open", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "ord", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "pow", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "range", -1, "(builtins)", CLASS_TYPE);
    insertsymbol(st, "round", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(st, "type", -1, "(builtins)", CLASS_TYPE);

    // Add string methods to string
    entry = insertsymbol(st, "str", -1, "(builtins)", CLASS_TYPE);
    entry->nested = mknested("(builtins)", -1, HASH_TABLE_SIZE, st, "class");
    insertsymbol(entry->nested, "replace", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(entry->nested, "split", -1, "(builtins)", FUNC_TYPE);
    
    // Add list methods to list
    entry = insertsymbol(st, "list", -1, "(builtins)", CLASS_TYPE);
    entry->nested = mknested("(builtins)", -1, HASH_TABLE_SIZE, st, "class");
    insertsymbol(entry->nested, "append", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(entry->nested, "remove", -1, "(builtins)", FUNC_TYPE);

    // Add file methods to file
    entry = insertsymbol(st, "file", -1, "(builtins)", CLASS_TYPE);
    entry->nested = mknested("(builtins)", -1, HASH_TABLE_SIZE, st, "class");
    insertsymbol(entry->nested, "read", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(entry->nested, "write", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(entry->nested, "close", -1, "(builtins)", FUNC_TYPE);;

    // Add dict methods to dict
    entry = insertsymbol(st, "dict", -1, "(builtins)", CLASS_TYPE);
    entry->nested = mknested("(builtins)", -1, HASH_TABLE_SIZE, st, "class");
    insertsymbol(entry->nested, "keys", -1, "(builtins)", FUNC_TYPE);
    insertsymbol(entry->nested, "values", -1, "(builtins)", FUNC_TYPE);

}

