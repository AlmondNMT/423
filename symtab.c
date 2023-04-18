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

    // Ensure that operand types are valid for arithmetic and logical expressions
    validate_operand_types(tree, st);
}

// Populate symbol tables from AST
void populate_symboltables(struct tree *t, SymbolTable st) {
    if (t == NULL || st == NULL) {
        return;
    }
    // Add the current symbol table to the current tree node
    t->stab = st;

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
    else if(strcmp(t->symbolname, "power") == 0) {
        if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
            check_decls(t->kids[0], st);
        }
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
    SymbolTableEntry entry = lookup(t->leaf->text, t->stab);
    if(entry == NULL || t->leaf->lineno < entry->lineno) {
        printf("fifa\n");
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
        entry->typ->u.f.returntype = alcbuiltin(ANY_TYPE);
        entry->typ->u.f.st = entry->nested;
    }
    
    // We will also annotate the tree node with this scope
    t->kids[0]->stab = st;
    t->kids[1]->stab = st;
    t->kids[2]->stab = st;
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
    struct typeinfo *type = NULL;

    // The fpdef nonterminal contains quite a bit of information about the parameter 
    // and its type, including its name
    if(strcmp(t->symbolname, "fpdef") == 0) {
        if(strcmp(t->kids[1]->symbolname, "colon_test_opt") == 0) {
            // The function param has a type hint
            type = get_fpdef_type(t->kids[1], ftable);
        }
        else {
            type = alcbuiltin(ANY_TYPE);
        }
        struct token *leaf = t->kids[0]->leaf;

        // Propagate type information to the parameter and typehint names
        t->kids[0]->type = type;
        t->kids[1]->type = type;
        t->type = type;
        insertsymbol(ftable, leaf->text, leaf->lineno, leaf->filename, type->basetype);
    } 
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], ftable);
        }
    }
    decorate_subtree_with_symbol_table(t, ftable);
}

/**
 * Traverse the rarrow subtree and add the type information to the entry.
 * We are starting at rarrow_opt here
 */
void add_func_type(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{
    if(entry == NULL || st == NULL || t == NULL)
        return;
    t->stab = st;
    // If we reach a power 
    struct typeinfo *typ = get_rhs_type(t, st);
    entry->typ->u.f.returntype = typ;
    decorate_subtree_with_symbol_table(t, st);
}

/**
 * Starting position "expr_stmt"
 * This handles function calls, assignments, and list accesses
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
    struct typeinfo *rhs_type = NULL, *lhs_type = NULL;
    if(strcmp(t->kids[1]->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        // Get the type of the rightmost branch by passing 'testlist' node
        rhs_type = get_rhs_type(t->kids[1]->kids[1], st);

        // Get the leftmost token first due to the shape of the tree
        struct token *leftmost = get_leftmost_token(t, st);

        // Add the leftmost op to the symbol table if it doesn't already exist, 
        //   then verify type compatibility with the rightmost operand
        entry = insertsymbol(st, leftmost->text, leftmost->lineno, leftmost->filename, ANY_TYPE);
        
        // Verify type compatible of LHS and RHS in assignment
        check_var_type(entry->typ, rhs_type, leftmost);

        // 
        add_nested_table(entry, rhs_type);

        // If there's any assignment chaining, verify the types of those 
        //   operands, and potentially add them to the table
        handle_eytr_chain(t->kids[1]->kids[0], st, rhs_type);
    }

    // Now we check the validity of augassigns (e.g., a += 1, b *= a, etc.)
    else if(strcmp(t->kids[1]->symbolname, "expr_conjunct") == 0) {
        //TODO: augassigns
    }

    // Function/constructor calls, list accesses, arithmetic expressions, dot 
    //   member accesses.
    // Assumption: expr_stmt only has one branch
    else {
        rhs_type = get_rhs_type(t->kids[0], st);
    }
    decorate_subtree_with_symbol_table(t, st);
}

/**
 * Verify type compatibility between LHS operands and the type of the rightmost
 * operand
 * TODO: Finishing variable type checking
 */
void check_var_type(struct typeinfo *lhs_type, struct typeinfo *rhs_type, struct token *tok)
{
    if(lhs_type == NULL || rhs_type == NULL) return;


    // If the right-hand-side of an assignment has type any, this is runtime's 
    //   problem
    if(lhs_type->basetype == ANY_TYPE || rhs_type->basetype == ANY_TYPE) return;

    // If the basetype of the entry is not ANY_TYPE, then check it against
    //   rhs type
    if(lhs_type->basetype != ANY_TYPE) {
        // TODO 
        if(lhs_type->basetype != rhs_type->basetype) {
            semantic_error(tok->filename, tok->lineno,
                    "incompatible assignment between '%s' and '%s' near operand '%s'\n", 
                    get_basetype(lhs_type->basetype), 
                    get_basetype(rhs_type->basetype),
                    tok->text
                    );
        }
    }

}

void add_nested_table(SymbolTableEntry entry, struct typeinfo *rhs_type)
{
    // Add the builtin/class symbol table to the entry if we passed previous checks
    // Determine which symbol table from the union to add
    if(rhs_type->basetype == FUNC_TYPE) entry->nested = rhs_type->u.f.st;
    else if(rhs_type->basetype == PACKAGE_TYPE) entry->nested = rhs_type->u.p.st;
    else entry->nested = rhs_type->u.cls.st;
    if(entry->nested != NULL) {
        entry->nested->parent = entry->table;
        entry->nested->level = entry->table->level + 1;
    }
}

/**
 * Auxiliary function for handling assignment chains
 */
void handle_eytr_chain(struct tree *t, SymbolTable st, struct typeinfo *rhs_type)
{
    if(t == NULL || st == NULL) return;
    if(strcmp(t->symbolname, "power") == 0) {
        handle_token(t, st);
        return;
    }
    // For nested EYTR assignment chains
    if(strcmp(t->symbolname, "equal_OR_yield_OR_testlist_rep") == 0) {
        handle_eytr_chain(t->kids[0], st, rhs_type);
        handle_eytr_chain(t->kids[1], st, rhs_type);
    }
    for(int i = 0; i < t->nkids; i++) {
        handle_eytr_chain(t->kids[i], st, rhs_type);
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
        // Add the typeinfo ptr to the leaf node
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
        if(strcmp(t->kids[0]->symbolname, "trailer_rep") == 0) {

            // If we could not find an entry in the table for the leftmost 
            //   token, throw a fit. For example, "a.b = 2" is only valid if 
            //   "a" is previously defined
            if(entry == NULL) {
                semantic_error(left->filename, left->lineno, "name '%s' is not defined\n", left->text);
            }
            
            // Get the nested symbol table of the entry, then call an auxiliary 
            //   function to locate the member being accessed. 
            printf("t->nkids: %d\n", t->nkids);
            entry = get_chained_dot_entry(t->kids[1], st, entry);
            
        }
        // TODO: If you want chained assignments to work, uncomment this line below
        entry = insertsymbol(st, left->text, left->lineno, left->filename, ANY_TYPE);
        
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

    // If we find that the first child is not a name, but the leaf 
    //   is not NULL 
    if(strcmp(t->symbolname, "NAME") != 0 && t->leaf != NULL)
        semantic_error(t->leaf->filename, t->leaf->lineno, "Cannot assign to literal\n");
    if(strcmp(t->symbolname, "atom") == 0) {
        // For invalid LHS list assignments
        if(strcmp(t->kids[0]->symbolname, "listmaker_opt") == 0)
            fprintf(stderr, "list created on LHS of assignment\n");
        // For invalid LHS dict assignments
        if(strcmp(t->kids[0]->symbolname, "dictorsetmaker_opt") == 0)
            fprintf(stderr, "dict created on LHS of assignment\n");
        exit(SEM_ERR);
    }
}   

void decorate_subtree_with_symbol_table(struct tree *t, SymbolTable st)
{
    SymbolTable nested = st;
    SymbolTableEntry entry = NULL;
    if(t == NULL || st == NULL) return;
    t->stab = st;
    if(strcmp(t->symbolname, "power") == 0) {
        if(strcmp(t->kids[0]->symbolname, "NAME") == 0) {
            struct token *tok = t->kids[0]->leaf;
            entry = lookup(tok->text, st);
            if(entry == NULL) 
                semantic_error(tok->filename, tok->lineno, "Name '%s' not found\n", tok->text);
            if(entry->typ->basetype != FUNC_TYPE && entry->nested != NULL) {
                nested = entry->nested;
            }
            if(entry->typ->basetype == FUNC_TYPE) { // TODO: Ensure all builtins have returntypes
                
            }
        }
    }
    for(int i = 0; i < t->nkids; i++)
        decorate_subtree_with_symbol_table(t->kids[i], nested);
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
    struct typeinfo *type = NULL;

    // Recurse until the "power" nonterminal is found
    if(strcmp(t->symbolname, "power") == 0 && t->kids[0]->leaf != NULL) {
        // 'power' can only be reached if we haven't already recursed to a 
        //   listmaker or a dictorset_option_* tree node 
        // This leaf contains the leftmost name of the expr_stmt
        struct token *leaf = t->kids[0]->leaf;

        // If the RHS token is a name
        if(leaf->category == NAME) {
            
            SymbolTableEntry entry = lookup(leaf->text, st);

            // Throw an error if entry could not be found
            if(entry == NULL) {
                semantic_error(leaf->filename, leaf->lineno, "Name '%s' is not defined\n", leaf->text);
            }
            struct typeinfo *trailer_type = NULL;
            if(strcmp(t->kids[1]->symbolname, "trailer_rep") == 0) {
                trailer_type = get_trailer_type(t->kids[1], st, entry);
                type = trailer_type;
                t->kids[1]->type = type;
            }
            else {
                type = type_copy(entry->typ);
            }
        }

        // If the RHS token is a literal, dict, list, bool, or None
        else {
            type = get_token_type(leaf);
        }
    } 

    // If we see listmaker_opt, we know that it's a list (e.g., [1, 2, b])
    else if(strcmp(t->symbolname, "listmaker_opt") == 0) {
        type = alclist();
    }

    // Dictionary
    else if(strcmp(t->symbolname, "dictorsetmaker_opt") == 0) {
        /* Right-hand side is a dictionary */
        type = alcdict();
    }
    else {
        /* It is assumed that we can just recurse the first child until one of 
         * the above three options is found 
         * TODO: Fix bad assumption */
        //printf("recursing: %s\n", t->symbolname);
        type = get_rhs_type(t->kids[0], st);
    }
    return type;
}

/**
 * Starting point: trailer_rep
*/
SymbolTableEntry get_rhs_entry(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{
    SymbolTableEntry lhs = NULL, rhs = NULL;
    struct token *tok = get_leftmost_token(t, st);
    printf("asdf%s\n", tok->text);

    return rhs;
}

/**
 * Assumption: Starting position is "trailer_rep"
 *   1. "arglist_opt": Function calls
 *   2. "subscriptlist": List/dict accesses
 *   3. "NAME": Dot operands
*/
struct typeinfo *get_trailer_type(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{   

    if(t == NULL || st == NULL ||  entry == NULL)
    {   
        fprintf(stderr, "ERROR get_trailer_type: one or more arguments is null\n");
        exit(SEM_ERR);
    }
    
    struct typeinfo *type = NULL;
    
    // If we find a subscript list anywhere, the type returned will be ANY_TYPE
    type = get_trailer_type_list(t, st);

    // If the return of the previous is not NULL just return it (ANY_TYPE)
    if(type != NULL) {
        return type;
    }

    // Assume function calls occur on the rightmost trailer, if they happen
   
    //if this is a function call, the node with the relevant type info is either
    // a sibling (if trailer_rep has no dots, like a plain function call f())
    // or it is in the first nested trailer_rep. 
    //so we just need to see if it's got a trailer rep child
    printf("here we are\n");
    if(is_function_call(t)) {
       // printf("IS FUNCTION CALL APPLIES\n");
        if(tr_has_tr_child(t))
        {
            type = t->kids[0]->kids[1]->kids[0]->type;
            printf("Dotted chain: %s, name %s\n", get_basetype(type->basetype), t->kids[0]->kids[1]->kids[0]->leaf->sval);
        }
        else 
        {  
            type = t->parent->kids[0]->type; //for debug for now, just do any, needs to change
            printf("NON-Dotted chain: %s name %s\n", get_basetype(type->basetype), t->parent->kids[0]->leaf->sval);
        }
       // printf("is thisnull\n");
    }

    

    // Hot fix: just make it ANY_TYPE to avoid the segfault
    if(type == NULL)
        return alcbuiltin(ANY_TYPE);
    return type;
}

/*struct typeinfo *get_type_of_node(struct tree *t, SymbolTable st, SymbolTableEntry entry)
{
   
}*/

int is_function_call(struct tree *t)
{   
    if(does_tr_have_trailer_child(t)) {

        //case: function call has arguments, then arglist is in inner node
        if(strcmp(t->kids[1]->kids[0]->symbolname, "arglist_opt") == 0) {
            printf("%s\n", t->parent->kids[0]->leaf->text);
            return 1;
        }
    }

    return 0;
}

int does_tr_have_trailer_child(struct tree *t) 
{
    if(strcmp(t->symbolname, "trailer_rep") == 0)
        if(t->kids[1] != NULL && strcmp(t->kids[1]->symbolname, "trailer") == 0)
            return 1;
    return 0;
}

//see if trailer rep has immediate trailer rep kid
//Assumption: we're feeding a confirmed trailer_rep node to this
int tr_has_tr_child(struct tree *t) 
{
        if(t->kids[0] != NULL && strcmp(t->kids[0]->symbolname, "trailer_rep") == 0)
            return 1;
    return 0;
}


struct typeinfo *get_trailer_type_list(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return NULL;
    struct typeinfo *type = NULL;
    if(strcmp(t->symbolname, "subscriptlist") == 0) {
        type = alcbuiltin(ANY_TYPE);
    }
    else {
        struct typeinfo *lhs = NULL, *rhs = NULL;
        lhs = get_trailer_type_list(t->kids[0], st);
        rhs = get_trailer_type_list(t->kids[1], st);
        if(lhs != NULL) {
            type = lhs;
        }
        if(rhs != NULL) {
            type = rhs;
        }
    }
    return type;
}


/** 
 * Get leftmost token in LHS of assignment
 */
struct token *get_leftmost_token(struct tree *t, SymbolTable st)
{
    struct token *tok = NULL;
    if(t == NULL || st == NULL) 
        return tok;
    if(strcmp(t->symbolname, "not_test") == 0) {
        printf("not_test: %d\n", t->nkids);
    }
    
    if(strcmp(t->symbolname, "power") == 0) {
        // Expect that the first power in the leftmost assignment is a NAME.
        // If it isn't a NAME or it's NULL, throw a syntax error
        
        tok = t->kids[0]->leaf;
        handle_token(t, st);
        printf("%s\n", tok->text);
        return tok;
    }
    else {
        return get_leftmost_token(t->kids[0], st);
    }
}


/**
 * Ensure that LHS and RHS of arithmetic/logical expressions 
 * are valid
 * This traverses the entire syntax tree looking
 * for 'or_test' nodes 
*/
void validate_operand_types(struct tree *t, SymbolTable st)
{  // printf("entering print tree\n");
    if(strcmp(t->symbolname, "or_test") == 0) {
        validate_or_test(t, st);
        return;
    }
    if(strcmp(t->symbolname, "and_test") == 0) {

        return;
    }

    for(int i = 0; i < t->nkids; i++) {
        validate_operand_types(t->kids[i], st);
    }
}

//one half of the two functions that should be able to handle
// nested combinations of or_tests (which expand to anything arithmetic and logical) 
//and trailer_reps
void handle_or_test_types(struct tree *t, SymbolTable st)
{  // printf("entering print tree\n");
    if(strcmp(t->symbolname, "or_test") == 0) {
        validate_or_test(t, st);
        return;
    }
    if(strcmp(t->symbolname, "and_test") == 0) {

        return;
    }

    for(int i = 0; i < t->nkids; i++) {
        validate_operand_types(t->kids[i], st);
    }
}


/**
* Whatever node of type 'test' was found
* will be validated in here. 
* Assumed starting position: or_test
*/
void validate_or_test(struct tree *t, SymbolTable st)
{   //if we find a power, and it has a child that is a NAME
    //we found a terminal and will return its typeinfo   
    // If the second child of or_test is "nulltree" don't check anything
    struct typeinfo *lhs_type = NULL, *rhs_type = NULL;
    if(strcmp(t->kids[1]->symbolname, "nulltree") == 0)
    {
        return;
    }
    else 
    {
        lhs_type = get_rhs_type(t, st);
        rhs_type = get_rhs_type(t->kids[1], st);
        printf("%s\n", get_basetype(lhs_type->basetype));
        printf("%s\n", get_basetype(rhs_type->basetype));
        if(strcmp(t->kids[1]->kids[0]->symbolname, "or_and_test_rep") == 0)
        {                
            validate_or_test(t->kids[1]->kids[0], st);
            validate_or_test(t->kids[1], st);
        }
    }

    return;
}



/** 
 * Add declarations to symbol table
 * Gather type info: 
 *  1. Check that the type declaration and optional assignment are consistent
 *  2. Add type information to SymbolTableEntry
 *
 *  Assumed start position: "decl_stmt"
 */
void get_decl_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return;

    // Decorate the tree with the current symbol table
    decorate_subtree_with_symbol_table(t, st);

    // The 'var' token has the NAME of the identifier
    // The 'type' token has the NAME of the type
    struct token *var = t->kids[0]->leaf;
    struct token *type = t->kids[1]->leaf;
    char *name = var->text;
    if(symbol_exists_current(name, st)) {
        // Redeclaration error
        semantic_error(var->filename, var->lineno, "Redeclaration for name '%s' in scope '%s'\n", name, st->scope);
    }

    // rhs_type is used for the case where an assignment is performed with a declaration
    struct typeinfo *rhs_type = get_ident_type(type->text, st);
    if(strcmp(t->kids[2]->symbolname, "equal_test_opt") == 0) {
        rhs_type = get_rhs_type(t->kids[2]->kids[1], st);
    }
    SymbolTableEntry e = insertsymbol(st, var->text, var->lineno, var->filename, rhs_type->basetype);
    check_var_type(e->typ, rhs_type, var);
    add_nested_table(e, rhs_type);
}

/**
 * Some boilerplate for searching the symbol tables for valid type hints,
 * then returning the corresponding type int
 */
struct typeinfo *determine_hint_type(struct token *type, SymbolTable st)
{
    if(type == NULL || st == NULL) {
        fprintf(stderr, "Why is type or st NULL?\n");
        exit(SEM_ERR);
    }
    struct typeinfo *typ = NULL;

    // type_entry is the RHS of "a: int", for example
    SymbolTableEntry type_entry = lookup(type->text, st);
    // If the type entry cannot be found in the symbol table, that's an error
    
    if(type_entry == NULL) 
        semantic_error(type->filename, type->lineno, "Name '%s' is not defined for the provided type\n", type->text);
    else {
        // When we find the type entry, we have to consider its type code. If it's 
        // a class, we obtain the class define code if it's a builtin, or ANY_TYPE 
        // otherwise.
        if(type_entry->typ->basetype == CLASS_TYPE) {
            typ = get_ident_type(type_entry->ident, st);

        } else {
            // If it's not a class type then let it inherit the type of the RHS
            typ = type_entry->typ;
        }
    }
    return typ;
}

/**
 * Get the function param type hint
 */
struct typeinfo *get_fpdef_type(struct tree *t, SymbolTable ftable)
{
    struct typeinfo *ret = NULL;
    if(t == NULL || ftable == NULL) // This probably should never happen
        return alcbuiltin(ANY_TYPE);
    if(strcmp(t->symbolname, "power") == 0) {
        ret = determine_hint_type(t->kids[0]->leaf, ftable);
        t->kids[0]->type = ret;
        return ret;
    } else {
        ret = get_fpdef_type(t->kids[0], ftable);
    }
    return ret;
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
        struct typeinfo *type = alcbuiltin(ANY_TYPE);
        t->kids[0]->type = type;
        insertsymbol(st, leaf->text, leaf->lineno, leaf->filename, type->basetype);
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
    t->kids[0]->stab = entry->nested;
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
    entry->typ->basetype = basetype; // All entries default to type ANY on the first pass
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

/** 
 * Wrapper function for insertsymbol that adds builtins
 */
SymbolTableEntry insertbuiltin(SymbolTable st, char *name, int lineno, char *filename, int basetype)
{
    if(st == NULL) return NULL;
    SymbolTableEntry entry = insertsymbol(st, name, lineno, filename, basetype);
    entry->typ->basetype = basetype;
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
            return "mystery type";
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
int get_ident_type_code(char *ident, SymbolTable st)
{
    return get_ident_type(ident, st)->basetype;
}


struct typeinfo *get_ident_type(char *ident, SymbolTable st)
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
    else {
        //here we know that it is NOT a builtin
        //so we look up the entry of said name
        //in an inside to outside direction (which is what lookup does)
        SymbolTableEntry type_entry = lookup(ident, st);
        if(type_entry != NULL) {
            //get type info if entry found
            return type_copy(type_entry->typ);
        }
        return alcbuiltin(ANY_TYPE);
    }
}


/**
 * Copy the typeptr of custom user classes
 * 
*/
struct typeinfo *type_copy(struct typeinfo *typ)
{
    // It is very unexpected if the typeptr is NULL
    if(typ == NULL) {
        fprintf(stderr, "This typeptr should not be null\n");
        exit(SEM_ERR);
    }

    struct typeinfo *copy = ckalloc(1, sizeof(struct typeinfo));
    // We only want to copy the symbol table of classes for 
    //   object instantiation, and not for functions and packages
    // if we assign a function f to a var a, like a = f, we only 
    // want to store a pointer. Same with packages.
    if(typ->basetype == FUNC_TYPE || typ->basetype == PACKAGE_TYPE) {
        return typ;
    }
    copy->u.cls.st = copy_symbol_table(typ->u.cls.st);
    return copy;
}


/**
* Make a copy of the symbol table, except leave the parent null
* parent will have to be assigned outside the scope of this 
* function
*/
SymbolTable copy_symbol_table(SymbolTable st)
{
    if(st == NULL) return NULL;
    SymbolTable copy  = mksymtab(st->nBuckets, st->scope);
    SymbolTableEntry old_entry = NULL, new_entry = NULL;
    for(int i = 0; i < st->nBuckets; i++) {
        if(st->tbl[i] != NULL) {
            old_entry = st->tbl[i];
            new_entry = insertsymbol(copy, old_entry->ident, old_entry->lineno, old_entry->filename, old_entry->typ->basetype);
            
            // If our current entry has its own nested symbol table
            if(old_entry->nested != NULL)
                new_entry->nested = copy_symbol_table(old_entry->nested);
        }
    }
    return copy;
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

    insertbuiltin(st, "any", -1, "(builtins)", CLASS_TYPE);

    entry = insertbuiltin(st, "print", -1, "(builtins)", FUNC_TYPE);
    entry->typ->u.f.returntype = alcnone();

    insertbuiltin(st, "None", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "int", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "abs", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "bool", -1, "(builtins)", CLASS_TYPE);  
    insertbuiltin(st, "chr", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "float", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "input", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "int", -1, "(builtins)", CLASS_TYPE);    
    insertbuiltin(st, "len", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "max", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "min", -1, "(builtins)", FUNC_TYPE);
    entry = insertbuiltin(st, "open", -1, "(builtins)", FUNC_TYPE);
    entry->typ->u.f.returntype = alcfile();
    insertbuiltin(st, "ord", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "pow", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "range", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "round", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "type", -1, "(builtins)", CLASS_TYPE);

    // Add string methods to string
    entry = insertbuiltin(st, "str", -1, "(builtins)", CLASS_TYPE);
    entry->typ = alcstr();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;
    
    // Add list methods to list
    entry = insertbuiltin(st, "list", -1, "(builtins)", CLASS_TYPE);
    // This adds "append" and "remove" to the list type symbol table
    entry->typ = alclist();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add file methods to file
    entry = insertbuiltin(st, "file", -1, "(builtins)", CLASS_TYPE);
    entry->typ = NULL;
    entry->typ = alcfile();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add dict methods to dict
    entry = insertbuiltin(st, "dict", -1, "(builtins)", CLASS_TYPE);
    free(entry->typ);
    entry->typ = (struct typeinfo *) alcdict();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

}
