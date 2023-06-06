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
#include "builtin_list.h"

extern tree_t* tree;
extern void add_puny_builtins(SymbolTable st);
extern char yyfilename[PATHMAX];
extern FILE *yyin;

// Global hash table for import names. Used to prevent circular imports
extern struct sym_table global_modules;

/**
 * Do semantic analysis here 
 */
void semantics(struct tree *tree, SymbolTable st, int add_builtins)
{
    if(tree == NULL || st == NULL) 
        return;

    // We first search for invalid expressions containing either functions or
    //  literals on the LHS of assignments. Python2.7 and Python3 both handle 
    //  invalid expressions before populating ST and type-checking. These 
    //  are syntactically valid, but don't make sense semantically.
    locate_invalid_expr(tree);
    
    // Add puny builtins like 'int', 'str' and 'open'
    if(add_builtins)
        add_puny_builtins(st);          

    // Populate symbol tables
    populate_symboltables(tree, st);   

    // (FOR DEBUGGING) verify that every node in the tree has a symtab
    //print_tree(tree, 0, 1);

    // Find names that are used, but not declared
    locate_undeclared(tree, st);       

    // Perform type-checking; function return types, function call argument 
    //   types, arithmetical operand types, chained dot operator accesses
    type_check(tree, st);
}

// Populate symbol tables from AST
void populate_symboltables(struct tree *t, SymbolTable st) {
    if (t == NULL || st == NULL) {
        return;
    }
    // Add the current symbol table to the current tree node
    t->stab = st;

    switch(t->prodrule) {
        // For functions, we add the name of the function to the current symbol 
        // table, then create a symbol table for the function.
        case FUNCDEF:
            insertfunction(t, st);
            return;

        // Class names
        case CLASSDEF:
            insertclass(t, st);
            return;
            
        // Global statements
        case GLOBAL_STMT:
            add_global_names(t, st);
            return;
            
        // Assignments
        case EXPR_STMT:
            handle_expr_stmt(t, st);
            return;

        // Import statements
        case DOTTED_AS_NAMES:
            get_import_symbols(t, st);
            return;

        case FOR_STMT:
            decorate_subtree_with_symbol_table(t, st);
            get_for_iterator(t, st);
            return;

        case DECL_STMT:
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
    switch(t->prodrule) {
        case DOTTED_AS_NAME: {
            // Don't check names in import stmts
            return;
        }
        // This case is to prevent further undeclared searches in trailers
        //   as that requires type info
        case POWER: {
            if(t->kids[0]->prodrule == NAME) {
                check_decls(t->kids[0], st);
            } 
            // Check for undeclared items in listmakers and dictmakers (e.g., [a, b], {a: 1, 2: b})
            else if(t->kids[0]->prodrule == ATOM) {
                locate_undeclared(t->kids[0], st);
            }
            return;
        }
        case NAME: {
            check_decls(t, st);
            break;
        }

    }
    for(int i = 0; i < t->nkids; i++) {
        // If the tree is annotated with symbol tables
        if(t->stab != NULL) 
            locate_undeclared(t->kids[i], t->stab);
        else
            locate_undeclared(t->kids[i], st);
    }
}


/**
 * Assumption: symbol is NAME. 
 *   
 * Throw an error if the symbol cannot be found in the symtab, or if it is an
 *   instance that occurs before its declared line number, or if it is used on 
 *   the same line that it is declared, and appears in a later column than where
 *   it's defined.
 */
void check_decls(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL || t->leaf == NULL) return;
    SymbolTableEntry entry = lookup(t->leaf->text, t->stab);

    // If the entry is not null, there are two conditions under which an undeclared
    //   error will be thrown. 
    //   1. NAME token is found on a line that precedes the location it was 
    //      defined on, and it isn't a function or class
    //   2. 
    if(entry == NULL || 
            (t->leaf->lineno < entry->lineno && 
             (entry->typ->basetype != FUNC_TYPE && entry->typ->basetype != CLASS_TYPE)) || 
            (t->leaf->lineno == entry->lineno && 
            t->leaf->column > entry->column)) {
        undeclared_error(t->leaf);
    }
}


/**
 * Get the global symtab;
 */
void add_global_names(tree_t *t, SymbolTable st)
{
    // Get the global symbol table
    SymbolTable global = get_global_symtab(st);
    if(t->nkids == 0) {
        return;
    }

    // The leaf 
    struct token *leaf = NULL;

    // The symbol name
    if(t->kids[0]->prodrule == NAME) {
        leaf = t->kids[0]->leaf;
        insertsymbol(global, leaf);
        add_global_names(t->kids[1], global);
    }
    else {
        leaf = t->kids[1]->leaf; 
        insertsymbol(global, leaf);
        add_global_names(t->kids[0], global);
    }
}

/**
 * Initialize the global_modules hash table
 */
void init_global_modules(SymbolTable global_modules)
{
    char *scope = "module";
    global_modules->nBuckets = HASH_TABLE_SIZE;
    global_modules->tbl = ckalloc(HASH_TABLE_SIZE, sizeof(SymbolTableEntry));
    global_modules->parent = NULL;
    global_modules->level = 0;
    global_modules->scope = ckalloc(strlen(scope) + 2, sizeof(char));
    strcpy(global_modules->scope, scope);
    insertmodule(global_modules, yyfilename);
}

/**
 * Add module to the global symbol table
 */
SymbolTableEntry insertmodule(SymbolTable st, char *modname)
{
    SymbolTableEntry entry = NULL;
    struct token *tok = create_token(modname, modname, 0, 1);
    entry = insertsymbol(st, tok);
    entry->typ->basetype = PACKAGE_TYPE;
    return entry;
}

/**
 * Insertfunction: Unlike insertsymbol, overwrite any previous definitions of 
 * s in the table. This means free the nested symbol table. populate_symboltables
 * will recursively descend through the tree to collect identifiers for its 
 * local scope. The returntype of the function should be initialized to 
 * ANY_TYPE. 
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
    SymbolTableEntry entry = insertsymbol(st, leaf);
    entry->typ->basetype = FUNC_TYPE;
    
    // In case a function was already defined, free its symbol table and 
    // make a new one
    free_symtab(entry->nested); 
    entry->nested = mknested(leaf->filename, leaf->lineno, HASH_TABLE_SIZE, st, "function");

    // The parent of the function's scope will be the current scope
    entry->nested->parent = st;

    // Pass the symbol table to every node in the rarrow branch
    decorate_subtree_with_symbol_table(t->kids[2], st);

    // Get function returntype 
    entry->typ->u.f.returntype = get_rhs_type(t->kids[2]);

    // Count the function parameters. Since user-def funcs have fixed params,
    //   min = max = nparams
    int nparams = get_func_param_count(t, 0);
    entry->typ->u.f.nparams = nparams;
    entry->typ->u.f.min_params = nparams;
    entry->typ->u.f.max_params = nparams;

    // We will also annotate the tree node with this scope
    t->kids[0]->stab = st;
    t->kids[1]->stab = st;
    t->kids[2]->stab = st;
    t->kids[3]->stab = entry->nested;

    get_function_params(t->kids[1], entry->nested);   // Add parameters to function scope
    populate_symboltables(t->kids[3], entry->nested); // Add suite to function scope

}

/**
 * Count the function parameters
 * Starting position: funcdef
*/
int get_func_param_count(struct tree *t, int count)
{
    if(t == NULL) return count;
    switch(t->prodrule) {
        case FUNCDEF: {
            return get_func_param_count(t->kids[1]->kids[0], count);
        }
        case VARARGSLIST: {
            int cnt = get_func_param_count(t->kids[0], count);
            cnt += get_func_param_count(t->kids[1], count);
            return cnt;
        }
        case FPDEF_OPTIONS:
        case FPDEF_EQUAL_TEST_COMMA_REP: {
            int cnt = get_func_param_count(t->kids[0], count);
            cnt += get_func_param_count(t->kids[1], count);
            return cnt;
        }
        case FPDEF: {
            return count + 1;
        }
    }
    return count;
}

/**
 * Starting position "expr_stmt"
 * This handles function calls, assignments, and list accesses. It's absolutely 
 *   massive.
 */
void handle_expr_stmt(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL)
        return;
    
    decorate_subtree_with_symbol_table(t, st);
    SymbolTableEntry entry = NULL;
    // At the highest level, there seems to be three kinds of expr_stmts: 
    //    1. Assignments (equal_OR_yield_OR_testlist_rep)
    //    2. Aug-assigns (expr_conjunct)
    //    3. Plain function calls, list accesses, and arithmetic expressions
    // EYTR indicates an assignment. It can be the second child of expr_stmt
    struct typeinfo *rhs_type = NULL;
    switch(t->kids[1]->prodrule) {
        case EQUAL_OR_YIELD_OR_TESTLIST_REP: {
            // Get the type of the RHS through the 'testlist' node
            rhs_type = get_rhs_type(t->kids[1]->kids[1]);

            struct token *leftmost = get_leftmost_token(t, st);

            // Add the leftmost op to the symbol table if it doesn't already exist, 
            entry = insertsymbol(st, leftmost);

            // Verify type compatible of LHS and RHS in assignment
            int compatible = are_types_compatible(entry->typ, rhs_type);
            if(!compatible)
                semantic_error(leftmost->filename, leftmost->lineno, "incompatible assignment between '%s' and '%s'\n", get_basetype_str(entry->typ->basetype), get_basetype_str(rhs_type->basetype));

            // Add the table in the rhs_type to the symbol entry
            add_nested_table(entry, rhs_type);

            // If there's any assignment chaining, verify the types of those 
            //   operands, and potentially add them to the table
            handle_eytr_chain(t->kids[1]->kids[0], st, rhs_type);
            break;
        }

        // Now we check the validity of augassigns (e.g., a += 1, b *= a, etc.)
        case EXPR_CONJUNCT: {
            // TODO: augassigns. These will most likely require a similar method
            //   as with arithmetic/logical operator checking. EXPR_CONJUNCT 
            //   appears only if there is an augassign
            break;
        }

        // Function/constructor calls, list accesses, arithmetic expressions, dot 
        //   member accesses.
        // Assumption: expr_stmt only has one branch
        default: {
            //rhs_type = get_rhs_type(t->kids[0]);
        }
    }
}

void add_nested_table(SymbolTableEntry entry, struct typeinfo *rhs_type)
{
    // Add the builtin/class symbol table to the entry if we passed previous checks
    // Determine which symbol table from the union to add
    if(rhs_type->basetype == FUNC_TYPE) {
        entry->nested = rhs_type->u.f.st;
        rhs_type->u.f.name = entry->ident;
    }
    else if(rhs_type->basetype == PACKAGE_TYPE) {
        entry->nested = rhs_type->u.p.st;
        rhs_type->u.p.name = entry->ident;
    }
    else {
        entry->nested = rhs_type->u.cls.st;
        rhs_type->u.cls.name = entry->ident;
    }
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
    if(t->prodrule == POWER) {
        handle_token(t, st);
        return;
    }
    // For nested EYTR assignment chains
    if(t->prodrule == EQUAL_OR_YIELD_OR_TESTLIST_REP) {
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
    switch(t->prodrule) {
        case TRAILER:
            break;
        case TRAILER_REP:
            break;
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
    if(t->prodrule == POWER) {
        // Then check if our first child is a NAME. If it is not, throw a fit,
        //   because this violates the assumption that only NAMES will be found,
        //   and not listmaker_opts, dictorsetmaker_opts, or literals
        if(t->kids[0]->prodrule != NAME) {
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
        if(t->kids[1]->prodrule == TRAILER_REP) {

            // If we could not find an entry in the table for the leftmost 
            //   token, throw a fit. For example, "a.b = 2" is only valid if 
            //   "a" is previously defined. 
            if(entry == NULL) {
                semantic_error(left->filename, left->lineno, "name '%s' is not defined\n", left->text);
            }
            
            // Get the nested symbol table of the entry, then call an auxiliary 
            //   function to locate the member being accessed. 
            entry = get_chained_dot_entry(t->kids[1], st, entry);
            
        }
        // TODO: If you want chained assignments to work, uncomment this line below
        entry = insertsymbol(st, left);
        
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
    if(t->prodrule == TRAILER_REP) {
        // DFS
        rhs = get_chained_dot_entry(t->kids[0], st, entry);
        // The first out of two possible cases: the child of "trailer" is a 
        //   name in a dot operation. 
        // In this example here, the second child of "trailer_rep",
        //   "t->kids[1]", is a "trailer". The first child of "trailer" is 
        //   either a "NAME" or a "subscriptlist".
        if(t->kids[1]->kids[0]->prodrule == NAME) {
            struct token *rhs_tok = t->kids[1]->kids[0]->leaf;

            // Get the symbol table of the rhs
            rhs = lookup_current(rhs_tok->text, entry->nested);
            if(rhs == NULL) {
                semantic_error(rhs_tok->filename, rhs_tok->lineno, "Name '%s' does not belong to class: %s\n", rhs_tok->text, get_basetype_str(entry->typ->basetype));
            }
        }

        // For lists
        else if(t->kids[1]->kids[0]->prodrule == SUBSCRIPTLIST) {
            
        }
        
        // For function/constructor calls
        else {
            
        }
    }
    return rhs;
}


/**
 * Search for expressions containing literals or function calls on the LHS of 
 * assignments. This can begin in the topmost node. This function's primary job 
 * is to restrict the permissiveness of the python2.7 syntax
 */
void locate_invalid_expr(struct tree *t)
{
    if(t == NULL) return;
    switch(t->prodrule) {
        case EXPR_STMT: {
            // Assignments with EQUAL signs are indicated by this nasty EYTR 
            //   nonterminal, which can be the second child of an expr_stmt
            if(t->kids[1]->prodrule == EQUAL_OR_YIELD_OR_TESTLIST_REP) {

                // Disallow chained assignments, e.g., a = b = 3
                if(t->kids[1]->kids[0]->prodrule == EQUAL_OR_YIELD_OR_TESTLIST_REP) {
                    fprintf(stderr, "Cannot perform chained assignment\n");
                    exit(SEM_ERR);
                }
                // We now search everywhere except the RHS for any literals, lists,
                //  or dicts, or arithmetic expressions
                locate_invalid_leftmost(t);
            }
            // Once we find an expr_stmt, we return to avoid traversing its 
            //   subtrees twice for no reason
            return;
        } 
        case RETURN_STMT: {
            // Return statements outside of function defs are invalid
            if(!has_ancestor(t, FUNCDEF)) {
                fprintf(stderr, "'return' outside function\n");
                exit(SEM_ERR);
            }
            break;
        }
        case CONTINUE_STMT: {
            if(!has_ancestor(t, WHILE_STMT) && !has_ancestor(t, FOR_STMT)) {
                fprintf(stderr, "'continue' not properly in loop\n");
                exit(SEM_ERR);
            }
            break;
        }
        case BREAK_STMT: {
            if(!has_ancestor(t, WHILE_STMT) && !has_ancestor(t, FOR_STMT)) {
                fprintf(stderr, "'break' outside loop\n");
                exit(SEM_ERR);
            }
        }
        default: {
        }
    }
    for(int i = 0; i < t->nkids; i++) {
        // Recurse all the children
        locate_invalid_expr(t->kids[i]);
    }
}

/**
 * Determine if funcdef is a parent of the current tree
*/
int has_ancestor(struct tree *t, int ANCESTOR)
{
    if(t == NULL) return false;
    if(t->prodrule == ANCESTOR) return true;
    return has_ancestor(t->parent, ANCESTOR);
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
    if(t->prodrule == POWER) {
        //   If we find a trailer nonterminal and it doesn't have a child NAME, 
        // throw an error saying 'cannot assign to function call'. This behavior
        // differs from Python's in that any puny function calls on the LHS of  
        // an assignment are erroneous, whereas Python only throws an error if 
        // the rightmost function call in a dotted trailer occurs.
        locate_invalid_trailer(t->kids[1], t->kids[0]->leaf);

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
    switch(t->prodrule) {
        case OR_TEST:
        case AND_TEST:
        case COMPARISON:
        case EXPR:
        case XOR_EXPR:
        case AND_EXPR:
        case SHIFT_EXPR:
        case ARITH_EXPR:
        case TERM:
            if(t->kids[1]->prodrule != NULLTREE) {
                struct token *leaf = get_expr_leaf(t);
                if(leaf != NULL) {
                    semantic_error(leaf->filename, leaf->lineno, "cannot assign to operator\n");
                }
                fprintf(stderr, "cannot assign to operator\n");
                exit(SEM_ERR);
            }
    }
}


/**
 * Starting point is either a nulltree, a trailer_rep, or a trailer 
 */
void locate_invalid_trailer(struct tree *t, struct token *tok)
{
    if(t == NULL) return;
    
    // If we find a trailer, we throw an error if it's first child is not a NAME
    switch(t->prodrule) {
        case TRAILER:
            // arglist_opt denotes a function call, which is illegal on the LHS of assignments
            switch(t->kids[0]->prodrule) {
                case ARGLIST_OPT:
                    fprintf(stderr, "%s:%d: Cannot assign to function call\n", tok->filename, tok->lineno);
                    exit(SEM_ERR);
                case NAME:
                    fprintf(stderr, "%s:%d: Cannot assign to object field\n", tok->filename, tok->lineno);
                    exit(SEM_ERR);
            }

        case TRAILER_REP:
            for(int i = 0; i < t->nkids; i++)
                locate_invalid_trailer(t->kids[i], tok);
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
    if(t->prodrule != NAME && t->leaf != NULL)
        semantic_error(t->leaf->filename, t->leaf->lineno, "Cannot assign to literal\n");
    if(t->prodrule == ATOM) {
        // For invalid LHS list assignments
        if(t->prodrule == LISTMAKER_OPT) {
            fprintf(stderr, "list created on LHS of assignment\n");
            exit(SEM_ERR);
        }
        // For invalid LHS dict assignments
        if(t->kids[0]->prodrule == DICTORSETMAKER_OPT) {
            fprintf(stderr, "dict created on LHS of assignment\n");
            exit(SEM_ERR);
        }
    }
}   

/**
 * Get a nearby token in an expression
 */
struct token *get_expr_leaf(struct tree *t)
{
    if(t == NULL) return NULL;
    struct token *tok = NULL;
    switch(t->prodrule) {
        case POWER:
            if(t->kids[0]->leaf != NULL) {
                tok = t->kids[0]->leaf;
                break;
            }
        default:
            for(int i = 0; i < t->nkids; i++) {
                tok = get_expr_leaf(t->kids[i]);
                if(tok != NULL) 
                    break;
            }
    }
    return tok;
}

/**
 * Add the appropriate symbol table to every node in the subtree
*/
void decorate_subtree_with_symbol_table(struct tree *t, SymbolTable st)
{
    SymbolTable nested = st;
    SymbolTableEntry entry = NULL;
    if(t == NULL || st == NULL) return;
    t->stab = st;
    switch(t->prodrule) {

        // Powers appear within EXPR_STMTs. 
        case POWER: {
            if(t->kids[0]->prodrule == NAME) {
                struct token *tok = t->kids[0]->leaf;
                entry = lookup(tok->text, st);
                if(entry != NULL) {

                    // If our entry has a nested symbol table, we want to decorate 
                    //   the subtrees with that one. I'm not sure this is being 
                    //   done correctly
                    if(entry->typ->basetype != FUNC_TYPE && entry->nested != NULL) {
                        nested = entry->nested;
                    }

                    // TODO: Ensure all builtins have returntypes
                    if(entry->typ->basetype == FUNC_TYPE) {

                    }
                }
            }
            break;
        }
        case FOR_STMT: {
            // This is here to add for statement iterators to the symtab
            get_for_iterator(t, st);
            break;
        }
    }
    for(int i = 0; i < t->nkids; i++)
        decorate_subtree_with_symbol_table(t->kids[i], nested);
}


int is_function_call(struct tree *t)
{   
    if(does_tr_have_trailer_child(t)) {

        //case: function call has arguments, then arglist is in inner node
        if(t->kids[1]->kids[0]->prodrule == ARGLIST_OPT) {
            //printf("%s\n", t->parent->kids[0]->leaf->text);
            return true;
        }
    }

    return false;
}

int does_tr_have_trailer_child(struct tree *t) 
{
    if(t->prodrule == TRAILER_REP)
        if(t->kids[1] != NULL && t->kids[1]->prodrule == TRAILER)
            return true;
    return false;
}

//see if trailer rep has immediate trailer rep kid
//Assumption: we're feeding a confirmed trailer_rep node to this
int tr_has_tr_child(struct tree *t) 
{
        if(t->kids[0] != NULL && t->kids[0]->prodrule == TRAILER_REP)
            return true;
    return false;
}


/** 
 * Get leftmost token in LHS of assignment
 */
struct token *get_leftmost_token(struct tree *t, SymbolTable st)
{
    struct token *tok = NULL;
    if(t == NULL || st == NULL) 
        return tok;
    if(t->prodrule == NOT_TEST) {
        printf("not_test: %d\n", t->nkids);
    }
    
    if(t->prodrule == POWER) {
        // Expect that the first power in the leftmost assignment is a NAME.
        // If it isn't a NAME or it's NULL, throw a syntax error
        
        tok = t->kids[0]->leaf;
        handle_token(t, st);
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
    if(t->kids[2]->prodrule == EQUAL_TEST_OPT) {
        rhs_type = get_rhs_type(t->kids[2]->kids[1]);
    }
    SymbolTableEntry e = insertsymbol(st, var);
    add_nested_table(e, rhs_type);
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
 * Name-not-found error
*/
void undeclared_error(struct token *tok)
{
    if(tok == NULL) return;
    semantic_error(tok->filename, tok->lineno, "Name '%s' not found\n", tok->text);
}

/*
Checks if given import_name is a built_in liberry
Depends on #include "builtin_list.h"
*/
int is_built_in(char *import_name) {
    
    
    // Create a pointer to iterate over the elements of builtin_list
    char **iterator = builtin_list;
    
    //will iterate until it finds LIST_END in the built in list, if it does return false
     while (*iterator != NULL) {
        
        //if import_name was found in built in list, return true
        if (strcmp(import_name, *iterator) == 0)
            return true;
        
        iterator++;
    }
    
    return false;
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
    SymbolTableEntry entry = NULL;
    strcpy(filename, import_name);
    strcat(filename, ".py");

    // If the file is not found in the current AND it is not a builtin, throw an error
    if(access(filename, F_OK) != 0 && !is_built_in(import_name)) {
        semantic_error(leaf->filename, leaf->lineno, "No module named '%s'\n", import_name);
    }

    // If we see an AS_NAME_OPT nonterminal we add the alias for the module
    if(dotted_as_name->kids[1]->prodrule == AS_NAME_OPT) { 
        struct token *alias = dotted_as_name->kids[1]->kids[0]->leaf;
        entry = insertsymbol(st, alias);
    } 

    // Otherwise just use the original name
    else {
        entry = insertsymbol(st, leaf);
    }

    // Insert package type information
    free_typeptr(entry->typ);
    entry->typ = NULL;
    entry->typ = alcbuiltin(PACKAGE_TYPE);
    
    // If the imported file exists in the current directory attempt to compile it
    if(access(filename, F_OK) == 0) {
        // If the filename is found in the global module hash table, throw an error
        if(module_exists(filename)) {
            fprintf(stderr, "%s: circular import for '%s'\n", yyfilename, filename);
            exit(SEM_ERR);
        } else {
            // Add module to hash table
            insertmodule(&global_modules, filename);
        }

        // Copy the current tree to a temporary variable, as it will be 
        //   overwritten by the following function call
        struct tree *current = tree, *tmp = NULL;
        
        
        // Preserving the original filename
        char *current_filename = strdup(yyfilename);
        
        // `tree` now points to the parse tree of the imported module
        get_imported_tree(filename);

        // Package symbol table
        int dont_add_builtins = 0;
        SymbolTable package_symtab = mknested(filename, entry->lineno, HASH_TABLE_SIZE, st, "package");
        entry->nested = package_symtab;

        // Point the package type symtab to the same region of memory
        entry->typ->u.p.st = package_symtab;
        entry->typ->u.p.name = entry->ident;
        semantics(tree, package_symtab, dont_add_builtins);

        // Revert the global tree
        tmp = tree;
        tree = current;
        
        // TODO: Generate transpiled code for packages with name mangling

        // Revert the global name back to the main source file.
        strcpy(yyfilename,  current_filename);
    }
}

void get_imported_tree(char *filename)
{

    // Initialize yyin for imported file
    check_access(filename);

    // Reset the global variables
    reset_globals();

    // Attempt to parse the imported file
    yyparse();

    // Prune the new tree
    prune_tree(tree, 0);

}

/**
 * Check if a module already exists in the global module hash table
 */
bool module_exists(char *filename)
{
    SymbolTableEntry entry = lookup(filename, &global_modules);
    if(entry != NULL && strcmp(filename, entry->ident) == 0) {
        return true;
    }
    return false;
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
    if(t->prodrule != POWER) {
        get_for_iterator(t->kids[0], st);
    } else {
        struct token *leaf = t->kids[0]->leaf;
        insertsymbol(st, leaf);
    }
    // Continue populating the subtrees of the `for` suite 
    for(int i = 0; i < t->nkids; i++) 
        populate_symboltables(t->kids[i], st);
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
    SymbolTableEntry entry = insertsymbol(st, leaf);

    free_symtab(entry->nested);
    entry->nested = mknested(leaf->filename, leaf->lineno, HASH_TABLE_SIZE, st, "class");
    
    free(entry->typ);

    // Create a new class with the given name (0 constructor params by default)
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
SymbolTableEntry insertsymbol(SymbolTable st, struct token *tok) {
    if(st == NULL || tok == NULL)
        return NULL;
        
    // Get the hash index of the given name then find it in the current scope if it exists.
    // If we find the symbol in this loop, we just return its entry. 
    char *name = tok->text;
    int lineno = tok->lineno;
    int column = tok->column;
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
    entry->typ = alcbuiltin(ANY_TYPE);
    //entry->typ->basetype = basetype; // All entries default to type ANY on the first pass
    entry->table = st;
    entry->ident = strdup(name);
    entry->lineno = lineno;
    entry->column = column;
    entry->filename = strdup(tok->filename);
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
            if(entry->typ->basetype == FUNC_TYPE && entry->typ->u.f.returntype != NULL) {
                printf("%s type ", get_basetype_str(entry->typ->basetype)); // Switch statements for base types
                printf("-> %s\n", get_basetype_str(entry->typ->u.f.returntype->basetype));
            }
            else 
                printf("%s type\n", get_basetype_str(entry->typ->basetype)); // Switch statements for base types
            if(entry->nested != NULL) {
                printsymbols(entry->nested);
            }
        }
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


            // I was going to free the goddamn typeptrs, but that caused all
            //   sorts of nihil for some reason

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
    if(name == NULL || st == NULL) 
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
    if(name == NULL || st == NULL) 
        return NULL;
    uint h = hash(st, name);
    SymbolTableEntry e = NULL;
    for(e = st->tbl[h]; e != NULL; e = e->next) {
        if(strcmp(e->ident, name) == 0)
            return e;
    }
    return e;
}

