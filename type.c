#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "nonterminal.h"
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"
#include "utils.h"
 
struct typeinfo none_type = { .basetype = NONE_TYPE, .u.cls.name = "none", .u.cls.returntype = &none_type };
struct typeinfo int_type = { .basetype = INT_TYPE, .u.cls.name = "int", .u.cls.returntype = &int_type };
struct typeinfo float_type = { .basetype = FLOAT_TYPE, .u.cls.name = "float", .u.cls.returntype = &float_type };
struct typeinfo bool_type = { .basetype = BOOL_TYPE, .u.cls.name = "bool", .u.cls.returntype = &bool_type };

struct typeinfo list_type = { .basetype = LIST_TYPE, .u.cls.name = "list", .u.cls.returntype = &list_type };
struct typeinfo dict_type = { .basetype = DICT_TYPE, .u.cls.name = "dict", .u.cls.returntype = &dict_type };
struct typeinfo string_type = { .basetype = STRING_TYPE, .u.cls.name = "str", .u.cls.returntype = &string_type};
struct typeinfo file_type = { .basetype = FILE_TYPE, .u.cls.name = "file", .u.cls.returntype = &file_type };

// We don't want a global any_type cuz the attached symbol tables are not fixed across variables
typeptr none_typeptr = &none_type;
typeptr int_typeptr = &int_type;
typeptr float_typeptr = &float_type;
typeptr bool_typeptr = &bool_type;
typeptr list_typeptr = &list_type;
typeptr dict_typeptr = &dict_type;
typeptr string_typeptr = &string_type;
typeptr file_typeptr = &file_type;

char *typenam[] =
   {"none", "int", "class", "list", "float", "func", "dict", "bool",
    "string", "package", "any"};


/**
 * Initialize some of the global primitive types
 */
void init_types()
{
    SymbolTable st = NULL;
    SymbolTableEntry entry  = NULL;

    list_typeptr->u.cls.st = mksymtab(HASH_TABLE_SIZE, "class");
    list_typeptr->u.cls.min_params = 0;
    list_typeptr->u.cls.max_params = 1;
    list_typeptr->u.cls.parameters = alcparam("a", alcbuiltin(ANY_TYPE));
    st = list_typeptr->u.cls.st;
    entry = insertbuiltin_meth(st, "append", none_typeptr);
    add_builtin_func_info(entry, 1, 1, none_typeptr, "%s: %d", "a", ANY_TYPE);
    entry = insertbuiltin_meth(st, "remove", none_typeptr);
    add_builtin_func_info(entry, 1, 1, none_typeptr, "%s: %d", "a", ANY_TYPE);

    dict_typeptr->u.cls.st = mksymtab(HASH_TABLE_SIZE, "class");
    dict_typeptr->u.cls.min_params = 0;
    dict_typeptr->u.cls.min_params = 1;
    dict_typeptr->u.cls.parameters = alcparam("a", alcbuiltin(ANY_TYPE));
    st = dict_typeptr->u.cls.st;
    entry = insertbuiltin_meth(st, "keys", list_typeptr);
    add_builtin_func_info(entry, 0, 0, list_typeptr, NULL);
    entry = insertbuiltin_meth(st, "values", list_typeptr);

    string_typeptr->u.cls.st = mksymtab(HASH_TABLE_SIZE, "class");
    string_typeptr->u.cls.min_params = 0;
    string_typeptr->u.cls.max_params = 1;
    string_typeptr->u.cls.parameters = alcparam("a", alcbuiltin(ANY_TYPE));
    st = string_typeptr->u.cls.st;
    entry = insertbuiltin_meth(st, "replace", string_typeptr);
    add_builtin_func_info(entry, 2, 2, string_typeptr, "%s: %d, %s: %d", "o", STRING_TYPE, "n", STRING_TYPE);
    entry = insertbuiltin_meth(st, "split", list_typeptr);
    add_builtin_func_info(entry, 1, 1, list_typeptr, "%s: %d", "c", STRING_TYPE);

    file_typeptr->u.cls.st = mksymtab(HASH_TABLE_SIZE, "class");
    file_typeptr->u.cls.min_params = 0;
    file_typeptr->u.cls.max_params = 1;
    file_typeptr->u.cls.parameters = alcparam("a", alcbuiltin(ANY_TYPE));
    st = file_typeptr->u.cls.st;
    entry = insertbuiltin_meth(st, "read", string_typeptr);
    add_builtin_func_info(entry, 0, 1, string_typeptr, "%s: %d", "n", INT_TYPE);
    entry = insertbuiltin_meth(st, "write", int_typeptr);
    add_builtin_func_info(entry, 1, 1, int_typeptr, "%s: %d", "s", STRING_TYPE);
}


char *typename(typeptr t)
{
   if (!t) return "(NULL)";
   else if (t->basetype < FIRST_TYPE || t->basetype > LAST_TYPE)
      return "(BOGUS)";
   else return typenam[t->basetype-1000000];
}


int type_str_to_int(char *typestr){

    if (strcmp(typestr, "class") == 0)
        return CLASS_TYPE;

    if (strcmp(typestr, "list") == 0)
        return LIST_TYPE;

    if (strcmp(typestr, "float") == 0)
        return FLOAT_TYPE;

    if (strcmp(typestr, "function") == 0)
        return FUNC_TYPE;

    if (strcmp(typestr, "dict") == 0)
        return DICT_TYPE;

    if (strcmp(typestr, "bool") == 0)
        return BOOL_TYPE;

    if (strcmp(typestr, "str") == 0)
        return STRING_TYPE;

    if (strcmp(typestr, "package") == 0)
        return PACKAGE_TYPE;

    if (strcmp(typestr, "any") == 0)
        return ANY_TYPE;

    if (strcmp(typestr, "file") == 0)
        return FILE_TYPE;
    return ANY_TYPE;
}

//returns the type for two operands based on operator or error
typeptr type_for_bin_op(typeptr lhs, typeptr rhs, struct token *tok)
{
    if(lhs == NULL || rhs == NULL || tok == NULL) semantic_error(tok, "Why are these types NULL???\n"); // This shouldn't happen

    typeptr ret = NULL;
    if(lhs->basetype == ANY_TYPE || rhs->basetype == ANY_TYPE) return alcbuiltin(ANY_TYPE);

    // We're expecting to see some kind of operator token here. We can default to 'invalid operator'
    // TODO: << >> | & 
    switch(tok->category) {
        case PLUS:
            ret = type_for_bin_op_plus(lhs, rhs);
            break;
        case MINUS:
            ret = type_for_bin_op_minus(lhs, rhs);
            break;
        case STAR:
            ret = type_for_bin_op_times(lhs, rhs);
            break;
        case SLASH:
            ret = type_for_bin_op_div(lhs, rhs);
            break;
        case DOUBLESTAR:
        case CIRCUMFLEX:
        case VBAR:
        case AMPER:
        case LEFTSHIFT:
        case RIGHTSHIFT:
            ret = type_for_bin_op_bitwise(lhs, rhs);
            break;
        case LESS:
        case GREATER:
        case LESSEQUAL:
        case GREATEREQUAL:
            ret = type_for_bin_op_great_less(lhs, rhs);
            break;
        case EQEQUAL:
            ret = type_for_bin_op_equals(rhs, lhs);
            break;
        case AND:
        case OR:
            ret = type_for_bin_op_logical(lhs, rhs);
            break;
        default: 
            semantic_error(tok, "unsupported operand type(s) for %s: '%s' and '%s'\n", tok->text, print_type(lhs), print_type(rhs));
    }
    if(ret == NULL) 
        semantic_error(tok, "unsupported operand type(s) for %s: '%s' and '%s'\n", tok->text, print_type(lhs), print_type(rhs));
    return ret;
}


typeptr type_for_bin_op_bitwise(typeptr lhs, typeptr rhs)
{
    if(lhs->basetype == INT_TYPE && rhs->basetype == INT_TYPE)
        return int_typeptr;
    return NULL;
}

//operand match check for plus
typeptr type_for_bin_op_plus(typeptr lhs, typeptr rhs)
{
    switch(lhs->basetype) {
        case INT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
        case FLOAT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return float_typeptr;
            }
            break;
        case BOOL_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;

        // TODO: Verify that these are possible to do in Unicon
        case STRING_TYPE:
            if(rhs->basetype == STRING_TYPE)
                return string_typeptr;
        case LIST_TYPE:
            if(rhs->basetype == LIST_TYPE)
                return list_typeptr;
    }
    return NULL;
}

//operand match check for minus
typeptr type_for_bin_op_minus(typeptr lhs, typeptr rhs)
{
    switch(lhs->basetype) {
        case INT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
        case FLOAT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return float_typeptr;
            }
            break;
        case BOOL_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
    }
    return NULL;
}

//operand match check for times
typeptr type_for_bin_op_times(typeptr lhs, typeptr rhs)
{
    switch(lhs->basetype) {
        case INT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
        case FLOAT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return float_typeptr;
            }
            break;
        case BOOL_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
    }
    return NULL;
}

//operand match check for div
typeptr type_for_bin_op_div(typeptr lhs, typeptr rhs)
{
    switch(lhs->basetype) {
        case INT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case BOOL_TYPE:
                    return int_typeptr;
                case FLOAT_TYPE:
                    return float_typeptr;
            }
            break;
        case FLOAT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return float_typeptr;
            }
            break;
        case BOOL_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return float_typeptr;
            }
    }
    return NULL;
}

//operand match check for ==, will always return bool because python is amazing
typeptr type_for_bin_op_equals(typeptr lhs, typeptr rhs)
{
        return bool_typeptr;
}

//operand match check for greater and less kind of operators
typeptr type_for_bin_op_great_less(typeptr lhs, typeptr rhs)
{
    switch(lhs->basetype) {
        case INT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return bool_typeptr;
            }
            break;
        case FLOAT_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return bool_typeptr;
            }
            break;
        case STRING_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case STRING_TYPE:
                    return bool_typeptr;
            }
            break;
        case BOOL_TYPE:
            switch(rhs->basetype) {
                case INT_TYPE:
                case FLOAT_TYPE:
                case BOOL_TYPE:
                    return bool_typeptr;
            }
            break;
        case LIST_TYPE:
            if(rhs->basetype == LIST_TYPE)
                return bool_typeptr;
            break;
    }
    return NULL;
}

//operand match check for logical operators
//as in python these behave totally different from normal 
//logical operators and types also depend on values
//this just returns any if its not expressly two bools
typeptr type_for_bin_op_logical(typeptr lhs, typeptr rhs)
{       
    if(lhs == NULL || rhs == NULL) return NULL;
    if(rhs->basetype == BOOL_TYPE && lhs->basetype == BOOL_TYPE)
        return bool_typeptr;
    return alcbuiltin(ANY_TYPE);
}

// builtin type allocations
typeptr alctype(int basetype)
{
    switch(basetype) {

    }
    typeptr ptr = ckalloc(1, sizeof(struct typeinfo));
    ptr->basetype = basetype;
    return ptr;
}

typeptr alcclass(char *name)
{
    typeptr ptr = alctype(CLASS_TYPE);
    ptr->u.cls.name = strdup(name);

    // The caller can determine whether it is an instance
    return ptr;
}

typeptr alcbuiltin(int basetype)
{
    switch(basetype) {
        case NONE_TYPE:
            return none_typeptr;
        case INT_TYPE:
            return int_typeptr;
        case LIST_TYPE:
            return list_typeptr;
        case FLOAT_TYPE:
            return float_typeptr;
        case DICT_TYPE:
            return dict_typeptr;
        case BOOL_TYPE:
            return bool_typeptr;
        case STRING_TYPE:
            return string_typeptr;
        case FILE_TYPE:
            return file_typeptr;
        case PACKAGE_TYPE:
            return alctype(PACKAGE_TYPE);
        case ANY_TYPE:
        default:
            return alctype(ANY_TYPE);
    }
}


paramlist alcparam(char *name, typeptr type)
{
    paramlist params = ckalloc(1, sizeof(struct param));
    params->name = strdup(name);
    params->type = type;
    params->next = NULL;
    return params;
}

/**
 * Starting position: root
 *
 * After associating types with identifiers, we have to perform checks similar
 *   in nature to those in the first phase of semantic analysis. It should now 
 *   be possible to verify all function/method calls (numargs = numparams and 
 *   type compatibility), 
 *
 * Another tree traversal for adding and checking type information
 * Starting position: root
 * Assumptions: The symbol table should be fully populated
 * TODO:
 *  ☑ Finish adding function type information
 *  ☑ Add class type information (NOT SUPPORTED)
 *  ☐ Propagate type information through assignments 
 *  ☐ Get type information for declarations
*/
void typecheck(struct tree *t)
{
    // Verify that functions are defined and used correctly
    //verify_correct_func_use(t, st);

    // Ensure that operand types are valid for arithmetic and logical expressions
    //validate_operand_types(t, st);

    switch(t->prodrule) {
        case EXPR_STMT:
            typecheck_expr_stmt(t);
            return;
        case DECL_STMT:
            typecheck_decl_stmt(t);
            return;
        case FUNCDEF:
            // Will traverse this again when checking DECL_STMTs and EXPR_STMTs
            typecheck_func_ret_type(t); 
            break;
        case FOR_STMT:
            typecheck_testlist(t->kids[1]);
            break;
        case WHILE_STMT:
            // TODO
            break;
        case IF_STMT:
            // TODO
            break;

    }
    for(int i = 0; i < t->nkids; i++) {
        typecheck(t->kids[i]);
    }

}


/**
 * For puny functions we want to guarantee the following things
 * 1. The return types and the actual returned value match
 * 2. The arguments and parameters are type-compatible
 * 3. Defined functions cannot be used without parentheses
 * 4. Only valid identifiers and types can be called
 *
 */
void verify_correct_func_use(struct tree *t, SymbolTable st)
{
    // Verify that argcount of function call matches formal param count
    //verify_func_arg_count(t);

    // Disallow function names from appearing within expr_stmts without parentheses
    disallow_funccall_no_parenth(t);

    // Verify that the return type in a function matches the stated 
    //   return type, if applicable
    typecheck_func_ret_type(t);

}

/**
 * For any functions in expr_stmts without parentheses, throw an error.
 * This prevents scenarios such as assigning functions to other variables, using
 */
void disallow_funccall_no_parenth(struct tree *t)
{
    if(t == NULL) return;
    switch(t->prodrule) {
        case EXPR_STMT: {
            for(int i = 0; i < t->nkids; i++)
                disallow_funccall_no_parenth_aux(t->kids[i]);
            return;
        }

    }
}

/**
 * Auxiliary function for disallow_funccall_np. 
 * Starting position: EXPR_STMT
 */
void disallow_funccall_no_parenth_aux(struct tree *t)
{
    switch(t->prodrule) {
        // When we find a POWER within an EXPR_STMT, we have to get 
        case POWER: {
            if(t->kids[0]->prodrule == NAME) {
                SymbolTableEntry entry = lookup(t->kids[0]->leaf->text, t->stab);
                if(entry == NULL)
                    undeclared_error(t->kids[0]->leaf);
                if(entry->typ->basetype == FUNC_TYPE) {
                    
                }
            }
            
        }
    }
}

/**
 * Starting position: root
*/
void verify_func_arg_count(struct tree *t)
{
    if(t == NULL) return;
    switch(t->prodrule) {
        // This indicates a function call
        case ARGLIST_OPT: {
            // Get the function ident
            struct token *ftok = t->parent->parent->parent->kids[0]->leaf;
            SymbolTableEntry entry = lookup(ftok->text, t->stab);
            // If entry is not found throw 'name not found' error
            if(entry == NULL)
                undeclared_error(ftok);

            // Use auxiliary function to count arguments
            int arg_count;

            // No-arg function call
            if(t->nkids == 0)
                arg_count = 0;
            else {
                // Count the function arguments starting from arglist
                arg_count = count_func_args(t->kids[0], 0);
            }

            // Check if the type is a class type (prolly a builtin)
            int param_count;
            // constructor param count
            if(entry->typ->basetype == CLASS_TYPE) {
                param_count = entry->typ->u.cls.nparams;
            }
            // regular function param count
            if(entry->typ->basetype == FUNC_TYPE)
                param_count = entry->typ->u.f.nparams;
            if(entry->typ->basetype == FUNC_TYPE || entry->typ->basetype == CLASS_TYPE) {
                if(arg_count < param_count) {
                    if(param_count - arg_count == 1)
                        semantic_error(ftok, "%s() missing 1 required positional argument\n", ftok->text);
                    else
                        semantic_error(ftok, "%s() missing %d required positional arguments\n", ftok->text, param_count - arg_count);
                }
                else if(arg_count > param_count) {
                    if(arg_count > 1)
                        semantic_error(ftok, "%s() takes %d positional arguments but %d were given\n", ftok->text, param_count, arg_count);
                    else
                        semantic_error(ftok, "%s() takes 0 positional arguments but 1 was given\n", ftok->text);
                }
            }
            
            return;
        }
    }
    for(int i = 0; i < t->nkids; i++) {
        verify_func_arg_count(t->kids[i]);
    }
}

/**
 * Auxiliary function for counting function/constructor arguments.
 * Starting from : arglist
*/
int count_func_args(struct tree *t, int count)
{
    switch(t->prodrule) {
        case ARGUMENT: {
            return count + 1;
        }
        default: {
            int cnt = 0;
            for(int i = 0; i < t->nkids; i++) {
                cnt += count_func_args(t->kids[i], count);
            }
            return cnt;
        }
    }
    return count;
}


/**
 * Ensure that listmaker contains only valid types
 * 
 * Starting point: listmaker_opt
 */
void typecheck_listmaker_contents(struct tree *t)
{
    if(t == NULL) return;
    typeptr type = NULL;
    struct token *desc = get_power_descendant(t); // If this turns up NULL there should be no errors;
    switch(t->prodrule){
        case LISTMAKER_OPT:
            typecheck_listmaker_contents(t->kids[0]);
        case LISTMAKER:
            type = typecheck_testlist(t->kids[0]);
            typecheck_listmaker_contents(t->kids[1]);
            break;
        case LISTMAKER_OPTIONS:
            typecheck_listmaker_contents(t->kids[0]);
                        break;
        case COMMA_TEST_REP:
            typecheck_listmaker_contents(t->kids[0]);
            type = typecheck_testlist(t->kids[1]);
            break;
    }
    if(type != NULL) {
        switch(type->basetype) {
            case CLASS_TYPE:
            case FUNC_TYPE:
            case PACKAGE_TYPE:
            case FILE_TYPE:
                semantic_error(desc, "Forbidden type '%s' found in list\n", print_type(type));
                break;
        }
    }
}

/**
 * Starting point: DICTORSETMAKER_OPT
 * TODO: Finish this
 */
void typecheck_dictmaker_contents(struct tree *t)
{
    if(t == NULL) return;
    typeptr type = NULL;
    struct token *desc = get_power_descendant(t); // Again, no worries if this returns NULL
    switch(t->prodrule) {
        case DICTORSETMAKER_OPT:
            break;
    }
}

/**
 * Get the type of a FACTOR nonterm
 *
 * If nested POWER type is non-numerical, throw an error
 */
typeptr typecheck_factor(struct tree *t)
{
    if(t == NULL) return alcbuiltin(ANY_TYPE);  // This shouldn't happen
    switch(t->prodrule) {
        case POWER:
            return typecheck_power(t);
    }
    typeptr type = NULL;
    type = typecheck_power(t->kids[1]);
    struct token *pm = t->kids[0]->leaf;

    // This handles the unary operators '+' and '-', only valid for the three types below
    switch(type->basetype) {
        case INT_TYPE:
        case FLOAT_TYPE:
        case ANY_TYPE:
            break;
        default:
            semantic_error(pm, "bad operand type for unary %s: '%s'\n", pm->text, print_type(type));
    }
    return type;
}

/**
 * Everything between a COMPARISON and a TERM has the exact same tree structure,
 *   so this function is used for all of them
 */
typeptr typecheck_op(struct tree *t)
{
    if(t == NULL) return NULL;
    typeptr lhs_type = NULL, rhs_type = NULL;
    lhs_type = typecheck_factor(t->kids[0]);
    rhs_type = typecheck_factor(t->kids[1]->kids[2]);
    struct token *op = t->kids[1]->kids[1]->leaf;
    return type_for_bin_op(lhs_type, rhs_type, op);
}

/**
 * Get the type of a POWER nonterm
 */
typeptr typecheck_power(struct tree *t)
{
    typeptr type = NULL, dstar_type = NULL;

    struct trailer *seq = NULL;
    // If we see an ATOM then we must traverse further to get the 
    //   type, because this indicates that we've encountered 
    //   a yield_expr_OR_testlist_comp, a listmaker_opt, or a 
    //   dictorsetmaker_opt
    if(t->kids[0]->prodrule == ATOM) {
        typeptr atom_type = get_rhs_type(t->kids[0]);
        // TODO: Type-check and get types for dicts/lists/parenthesized expressions
        if(t->kids[1]->prodrule == TRAILER_REP) {
            seq = build_trailer_sequence(t->kids[1]);
            print_trailer_sequence(seq);
        }
        return get_rhs_type(t->kids[0]);
    }
    
    // This leaf contains the leftmost name of the expr_stmt
    struct token *leaf = t->kids[0]->leaf;

    // TODO Get dstar type e.g., a ** b
    if(t->kids[2]->prodrule == DSTAR_FACTOR_OPT) {
        
    }

    // If the POWER's first child is a leaf, then we must get the immediate type
    if(t->kids[0]->leaf != NULL) {
        // 'power' can only be reached if we haven't already recursed to a 
        //   listmaker or a dictorset_option_* tree node 

        // If the RHS token is a name
        if(leaf->category == NAME) {
            
            SymbolTableEntry entry = lookup(leaf->text, t->stab);

            // Throw an error if entry could not be found
            if(entry == NULL) {
                undeclared_error(leaf);
            }

            // Forget about trailer_reps here, just get the identifier type
            type = get_ident_type(entry->ident, t->stab);
            
            // If there is a TRAILER_REP present, do the difficult thing
            if(t->kids[1]->prodrule == TRAILER_REP) {
                // Build a linked list sequence of trailers
                seq = build_trailer_sequence(t->kids[1]);

                /*printf("%s", leaf->text);
                print_trailer_sequence(seq);*/

                type = get_trailer_rep_type(seq, entry, leaf);
                free_trailer_sequence(seq);
            }
            else {
                type = entry->typ;
                if(type->basetype == FUNC_TYPE || type->basetype == CLASS_TYPE) {
                    semantic_error(leaf, "function/class use with no parentheses\n");
                }
            }
        }

        // If the RHS token is a literal, dict, list, bool, or None
        else {
            type = get_token_type(leaf);

            // Make literals with trailers INVALID
            if(t->kids[1]->prodrule == TRAILER_REP) {
                const char *typename = print_type(type);
                switch(t->kids[1]->kids[1]->kids[0]->prodrule) {
                    case ARGLIST_OPT:
                        semantic_error(leaf, "'%s' type is not callable\n", typename);
                        break;
                    case SUBSCRIPTLIST:
                        semantic_error(leaf, "'%s' type is not subscriptable\n", typename);
                        break;
                    case NAME:
                        semantic_error(leaf, "invalid field access for '%s'\n", typename);
                        break;
                }
                semantic_error(leaf, "'%s' object is not callable/subscriptable\n", print_type(type)); 
            }
        }
    }
    return type;
}


void print_trailer_sequence(struct trailer *seq)
{
    if(seq == NULL) {printf("\n"); return;}
    switch(seq->prodrule) {
        case NAME:
            printf(".%s", seq->name);
            break;
        case ARGLIST_OPT:
            printf("(");
            print_arglist(seq->arg);
            printf(")");
            break;
        case SUBSCRIPTLIST:
            printf("[");
            print_arglist(seq->arg);
            printf("]");
            break;
        default:
            printf("???");
    }
    print_trailer_sequence(seq->next);
}

void print_arglist(struct arg *arg)
{
    if(arg == NULL) return;
    if(arg->next == NULL) {
        printf("%s", print_type(arg->type));
    }
    else {
        printf("%s, ", print_type(arg->type));
        print_arglist(arg->next);
    }
}

/**
 * Transform the nasty recursive tree problem into a sequential one
 *
 * Starting position: trailer_rep
 *
 * This recurses through the tree to obtain a sequential representation of the 
 *   trailers, making it easier to obtain the final type
 */
struct trailer *build_trailer_sequence(struct tree *t)
{
    if(t == NULL) return NULL;
    struct trailer *prev = NULL, *next = NULL;
    if(t->kids[0]->prodrule == TRAILER_REP) {
        prev = build_trailer_sequence(t->kids[0]);
    } 

    // If the first child is a NULLTREE, assume that the right child is a TRAILER
    int code = 0;
    char *name = NULL;
    if(t->kids[1]->kids[0]->prodrule == NAME) {
        name = t->kids[1]->kids[0]->leaf->text;
    }
    code = t->kids[1]->kids[0]->prodrule;
    next = create_trailer_link(name, code);

    // Get the arglist of the function call or list access to verify correctness
    next->arg = build_arglist(t->kids[1]->kids[0]); // We are passing in either a 
    if(prev != NULL) {
        struct trailer *curr = prev;
        while(curr->next != NULL) curr = curr->next;
        curr->next = next;
        return prev;
    }
    return next;
}

/**
 * Starting position: either ARGLIST_OPT or SUBSCRIPTLIST
 */
struct arg *build_arglist(struct tree *t)
{
    if(t == NULL) return NULL;
    struct arg *prev = NULL, *next = NULL;
    typeptr type = NULL;

    // The tree structure for these two nonterminals is only slightly different, 
    switch(t->prodrule) {
        case ARGLIST_OPT:
            // ARGLIST_OPT -> ARGLIST/NULLTREE ->
            prev = build_arglist(t->kids[0]);
            break;
        case ARGLIST:
            // ARGLIST -> ARG_COMMA_REP
        case SUBSCRIPTLIST: 
        case SUBSCRIPT_COMMA_REP:
        case ARG_COMMA_REP:
            // SUBSCRIPTLIST -> SUBSCRIPT_COMMA_REP
            prev = build_arglist(t->kids[0]);
            type = typecheck_testlist(t->kids[1]->kids[0]);
            //printf("%s\n", print_type(type));
            next = create_arg_link(type);
            break;
    }

    if(prev != NULL) {
        struct arg *curr = prev;
        while(curr->next != NULL) curr = curr->next;
        curr->next = next;
        return prev;
    }
    return next;
}

/**
 * Build a single trailer link for the trailer linked list
 */
struct trailer *create_trailer_link(char *name, int prodrule)
{
    struct trailer *node = ckalloc(1, sizeof(struct trailer));
    if(name != NULL) 
        node->name = strdup(name);
    node->prodrule = prodrule;
    return node;
}

/**
 * Build a single arg link for the arg linked list
 */
struct arg *create_arg_link(typeptr type)
{
    if(type == NULL) { fprintf(stderr, "Why is type null? This shouldn't happen\n"); exit(SEM_ERR); }
    struct arg *node = ckalloc(1, sizeof(struct arg));
    node->type = type;
    return node;
}

void free_trailer_sequence(struct trailer *seq)
{
    if(seq == NULL) return;
    free_trailer_sequence(seq->next);
    free(seq->arg);
    free(seq->name);
    free(seq);
}

void free_arglist(struct arg *arg)
{
    if(arg == NULL) return;
    //free_typeptr(arg->type);
    free_arglist(arg->next);
    free(arg);
}

/**
 * Starting position: TRAILER_REP
 *
 * These things are so complicated, but the basic idea is that they can
 *   generate a sequence of various function calls, list accesses, and chains of
 *   dotted operators. e.g., a = a.b[0]().c
 */
struct typeinfo *get_trailer_rep_type(struct trailer *seq, SymbolTableEntry entry, struct token *tok)
{
    // We can probably assume everything is non-null
    if(seq == NULL || entry == NULL || tok == NULL) {
        fprintf(stderr, "get_trailer_rep_type: at least one of the arguments is NULL\n");
        exit(SEM_ERR);
    }

    // Bunch of initialization. Might be a more efficient way to do all of this, but I don't care
    struct trailer *curr = NULL, *start = create_trailer_link(entry->ident, NAME);
    start->next = seq;
    SymbolTableEntry rhs = entry; // Initialize rhs with the entry pointer
    SymbolTable nested = entry->nested;
    typeptr current_type = entry->typ;

    int arg_count = 0;

    for(curr = start->next; curr != NULL; curr = curr->next) {
        const char *type_name = print_type(current_type);
        switch(curr->prodrule) {
            // For each of three possible trailers, we ensure that 
            case NAME:
                switch(current_type->basetype) {
                    case FUNC_TYPE:
                    case CLASS_TYPE:
                    case NONE_TYPE:
                    case INT_TYPE:
                    case FLOAT_TYPE:
                    case BOOL_TYPE:
                        semantic_error(tok, "invalid field access for type '%s'\n", type_name);
                }
                rhs = lookup_current(curr->name, nested);
                if(rhs == NULL) {
                    semantic_error(tok, "'%s' object has no attribute '%s'\n", type_name, curr->name);
                }
                nested = rhs->typ->u.cls.st;
                current_type = rhs->typ;
                break;
            case ARGLIST_OPT:
                switch(current_type->basetype) {
                    // Functions or classes
                    case FUNC_TYPE:
                    case CLASS_TYPE:
                        current_type = rhs->typ->u.f.returntype;
                        nested = current_type->u.cls.st;
                        arg_count = count_args(curr->arg);
                        // max_params == -1 implies 0 ... n possible arguments of ANY_TYPE
                        if(rhs->typ->u.f.max_params != -1) {
                            if(rhs->typ->u.f.min_params == rhs->typ->u.f.max_params && arg_count != rhs->typ->u.f.min_params)
                                semantic_error(tok, "'%s' takes at exactly %d parameter(s), but %d was/were given\n",
                                        rhs->ident, rhs->typ->u.f.min_params, arg_count);
                            if(arg_count > rhs->typ->u.f.max_params)
                                semantic_error(tok, "'%s' takes at most %d parameter(s), but %d was/were given\n", 
                                        rhs->ident, rhs->typ->u.f.max_params, arg_count);
                            if(arg_count < rhs->typ->u.f.min_params)
                                semantic_error(tok, "'%s' takes at least %d parameter(s), but %d was/were given\n",
                                        rhs->ident, rhs->typ->u.f.min_params, arg_count);
                        }
                        check_args_with_params(curr->arg, rhs->typ->u.f.parameters, tok, 1);
                        break;
                    default:
                        semantic_error(tok, "'%s' object is not callable\n", type_name);

                }
                break;

            // Verify list/dict accesses. Return any_type. The rest of type-checking here should be done at runtime.
            case SUBSCRIPTLIST:

                // There should be exactly one subscript for both lists and dicts
                arg_count = count_args(curr->arg);
                if(arg_count != 1) semantic_error(tok, "must have exactly one subscript\n");
                switch(current_type->basetype) {
                    case LIST_TYPE:
                        // Ensure that the single argument is an integer (or ANY_TYPE)
                        if(curr->arg->type->basetype != INT_TYPE && curr->arg->type->basetype != ANY_TYPE)
                            semantic_error(tok, "list index must be 'int', not '%s'\n", print_type(curr->arg->type));
                        break;
                    case DICT_TYPE:
                        // TODO Ensure that the argument type is either an int, a float or a string
                        switch(curr->arg->type->basetype){
                            case INT_TYPE:
                            case FLOAT_TYPE:
                            case STRING_TYPE:
                                break;
                            default:
                                semantic_error(tok, "dict index cannot be '%s'\n", print_type(curr->arg->type));
                        }
                        break;
                    case ANY_TYPE:
                        break;
                    default:
                        semantic_error(tok, "'%s' object is not subscriptable\n", type_name);
                }
                return alcbuiltin(ANY_TYPE);
                break;
        }
    }
    start->next = NULL;
    free_trailer_sequence(start);
    if(current_type == NULL) 
        return alcbuiltin(ANY_TYPE);
    return current_type;
}


/**
 * Checking the types of the parameters against the arguments
 * Assumption: len(params) >= len(args)
 */
void check_args_with_params(struct arg *args, struct param *params, struct token *tok, int count)
{
    if(args == NULL || params == NULL) return;
    if(!are_types_compatible(params->type, args->type))
        semantic_error(tok, "'%s' requires '%s' for arg %d, but '%s' was given\n", tok->text, print_type(params->type), count, print_type(args->type));
    check_args_with_params(args->next, params->next, tok, count + 1);
}

int count_args(struct arg *arg)
{
    if(arg == NULL) return 0;
    return 1 + count_args(arg->next);
}


/**
 * Try to find a POWER ancestor with a NAME child, then return the token
 */
struct token *get_power_ancestor(struct tree *t)
{
    if(t == NULL) return NULL;
    struct token *power_name = NULL;
    if(t->prodrule == POWER && t->kids[0]->prodrule == NAME) {
        power_name = t->kids[0]->leaf;
    }
    else {
        power_name = get_power_ancestor(t->parent);
    }
    return power_name;
}


/**
 * Free function/constructor parameters
*/
void free_params(paramlist params)
{
    if(params == NULL)
        return;
    free_params(params->next);
    free(params);
}

void free_typeptr(typeptr typ)
{
    if(typ == NULL) return;
    switch(typ->basetype) {
        case FUNC_TYPE:
            free_typeptr(typ->u.f.returntype);
            free_params(typ->u.f.parameters);
            break;

        case PACKAGE_TYPE:
            break;

        default:
            free_params(typ->u.cls.parameters);
            break;
    }
    free(typ);
}

/**
 * Get a struct token *leaf from somewhere below (it doesn't matter which one)
 */
struct token *get_power_descendant(struct tree *t) 
{
    if(t == NULL) return NULL;
    struct token *tok = NULL;
    if(t->leaf != NULL)
        return t->leaf;
    for(int i = 0; i < t->nkids; i++) {
        tok = get_power_descendant(t->kids[i]);
        if(tok != NULL) break;
    }
    return tok;
}

/**
 * Starting position: EXPR_STMT
 *
 * We want to add type information to the tree for the purposes of type-checking
 * assignments and arithmetic/logical expressions.
 */
void typecheck_expr_stmt(struct tree *t)
{
    if(t == NULL) return;
    typeptr lhs_type = NULL, rhs_type = NULL;

    // There are 3 main scenarios with EXPR_STMTS
    //   1. Non-assignment expression: LHS
    //   2. Assignment expression    : LHS = RHS
    //   3. Augmented assignment     : LHS += RHS
    // First, we can get the LHS_TYPE. In all three cases, this type-checks the LHS expression
    //   i.e., 
    lhs_type = typecheck_testlist(t->kids[0]); 
    
    // Secondly, determine the scenario: proceed if second child isn't NULLTREE
    switch(t->kids[1]->prodrule) {
        case EQUAL_OR_YIELD_OR_TESTLIST_REP:

            // The type on the right
            rhs_type = typecheck_testlist(t->kids[1]->kids[1]);

            if(!are_types_compatible(lhs_type, rhs_type)) {
                // Grab nearby descendant token and use it for assignment type-check error
                struct token *desc = get_power_descendant(t->kids[0]);
                const char *left = print_type(lhs_type);
                const char *right = print_type(rhs_type);
                if(desc != NULL) {
                    semantic_error(desc, "incompatible assignment between '%s' and '%s'\n", left, right);
                }
            }
            break;
        case EXPR_CONJUNCT:

            break;
        default:

            break;
    }
}

/**
 * Starting point: Could be any number of things between TESTLIST and POWER,
 *   depending on how the tree is pruned.
 */
typeptr typecheck_testlist(struct tree *t)
{
    if(t == NULL) return NULL;
    typeptr type = NULL;
    // Note: the TEST nonterminal never presents
    switch(t->prodrule) {
        case TESTLIST:
            break;
        case OR_TEST:
        case AND_TEST:
        case COMPARISON:
        case EXPR:
        case XOR_EXPR:
        case AND_EXPR:
        case SHIFT_EXPR:
        case ARITH_EXPR:
        case TERM:
            type = typecheck_op(t);
            break;
        case FACTOR:
            type = typecheck_factor(t);
            break;
        case POWER:
            type = typecheck_power(t);
            break;
    }
    return type;
}

/**
 * Starting point: DECL_STMT
*/
void typecheck_decl_stmt(struct tree *t)
{
    if(t == NULL) return;
    SymbolTableEntry lhs = lookup(t->kids[0]->leaf->text, t->stab);

    // Free the default ANY_TYPE 
    free_typeptr(lhs->typ);

    // Assign type to LHS
    lhs->typ = get_ident_type(t->kids[1]->leaf->text, t->stab);

    // Verify that type assignment matches
    if(t->kids[2]->prodrule == EQUAL_TEST_OPT) {
        typeptr assignment_type = NULL, decl_type = NULL;

        // Pass the second child of the equal_test_opt, cuz the 
        //   first child is '='
        assignment_type = typecheck_testlist(t->kids[2]->kids[1]);
        decl_type = lhs->typ;
        if(!are_types_compatible(decl_type, assignment_type)) {
            const char *left = print_type(decl_type);
            const char *right = print_type(assignment_type);
            struct token *tok = t->kids[0]->leaf;
            semantic_error(tok, "Incompatible assignment between '%s' and '%s'\n", left, right);
        }

    }
}

/**
 * Get the function param type hint
 *
 * Starting point: COLON_NAME_OPT
 */
struct typeinfo *get_fpdef_type(struct tree *t, SymbolTableEntry entry)
{
    struct typeinfo *ret = NULL;
    if(t == NULL || entry == NULL) // This probably should never happen
        return alcbuiltin(ANY_TYPE);
    ret = get_ident_type(t->kids[0]->leaf->text, entry->nested);
    t->kids[0]->type = ret;
    t->type = ret;
    return ret;
}

/**
 * Starting from the parameters rule, navigate to fpdef_equal_test_comma_rep,
 * then recurse the descendants. 
 * fpdef has two children
 * 
 * The entry is for the function itself
 */
void get_function_params(struct tree *t, SymbolTableEntry fentry)
{
    if(t == NULL || fentry == NULL)
        return;
    // This is the default base type
    struct typeinfo *type = NULL;

    // The fpdef nonterminal contains information about the parameter 
    //   and its type, including its name
    if(t->prodrule == FPDEF) {
        if(t->kids[1]->prodrule == COLON_NAME_OPT) {
            // The function param has a type hint
            type = get_fpdef_type(t->kids[1], fentry);
        }
        else {
            type = alcbuiltin(ANY_TYPE);
        }
        struct token *leaf = t->kids[0]->leaf;

        // Propagate type information to the parameter and typehint names
        t->kids[0]->type = type;
        t->kids[1]->type = type;
        t->type = type;
        SymbolTableEntry param_entry = insertsymbol(fentry->nested, leaf);
        //free_typeptr(entry->typ);
        param_entry->typ = type;
        add_param_to_function_entry(leaf, type, fentry);
    } 
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], fentry);
        }
    }
}


/**
 * This populates the paramlist in the function
 */
void add_param_to_function_entry(struct token *param_name_tok, typeptr param_typ, SymbolTableEntry fentry)
{
    if(param_name_tok == NULL || param_typ == NULL || fentry == NULL) return;
    paramlist param = alcparam(param_name_tok->text, param_typ);
    if(fentry->typ->u.f.parameters == NULL) 
        param = fentry->typ->u.f.parameters = param;
    else {
        paramlist curr = fentry->typ->u.f.parameters;
        while(curr->next != NULL) curr = curr->next;
        curr->next = param;
    }
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
        return list_typeptr;
    else if(strcmp(ident, "float") == 0)
        return float_typeptr;
    else if(strcmp(ident, "dict") == 0)
        return dict_typeptr;
    else if(strcmp(ident, "bool") == 0)
        return bool_typeptr;
    else if(strcmp(ident, "str") == 0)
        return string_typeptr;
    else if(strcmp(ident, "None") == 0)
        return none_typeptr;
    else if(strcmp(ident, "any") == 0)
        return alcbuiltin(ANY_TYPE);
    else if(strcmp(ident, "file") == 0)
        return file_typeptr;
    else {
        // here we know that it is NOT a builtin
        // so we look up the entry of the name
        // from the inner symtab to the outer
        SymbolTableEntry type_entry = lookup(ident, st);
        if(type_entry != NULL) {
            //get type info if entry found
            return type_entry->typ;
        }
        else {
            fprintf(stderr, "'%s' not found\n", ident);
            exit(SEM_ERR);
        }
        printf("%d\n", ANY_TYPE);
        return alcbuiltin(ANY_TYPE);
    }
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
        semantic_error(type, "Name '%s' is not defined for the provided type\n", type->text);
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
 * Copy the typeptr of custom user classes
 * 
*/
struct typeinfo *type_copy(struct typeinfo *typ)
{
    // End of recursion
    if(typ == NULL) {
        return NULL;
    }
    printf("typecopy: %s\n", print_type(typ));
    // We only want to copy the symbol table of classes for 
    //   object instantiation, and not for functions and packages
    // if we assign a function f to a var a, like a = f, we only 
    // want to store a pointer. Same with packages.
    if(typ->basetype == FUNC_TYPE || typ->basetype == PACKAGE_TYPE) {
        return typ;
    }

    // Instantiating an object
    struct typeinfo *copy = ckalloc(1, sizeof(struct typeinfo));
    copy->basetype = typ->basetype;
    copy->u.cls.nparams = typ->u.cls.nparams;
    copy->u.cls.parameters = copy_params(typ->u.cls.parameters);
    copy->u.cls.st = copy_symbol_table(typ->u.cls.st);
    copy->u.cls.returntype = type_copy(typ->u.cls.returntype);
    return copy;
}

/**
* Make a copy of the symbol table, except leave the parent null
* parent will have to be assigned outside of this 
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
            struct token *tok = create_token(old_entry->ident, old_entry->filename, old_entry->lineno, old_entry->column);
            new_entry = insertsymbol(copy, tok);
            
            // If our current entry has its own nested symbol table
            if(old_entry->nested != NULL) {
                new_entry->nested = copy_symbol_table(old_entry->nested);
                printf("Copying the nested symbol table of %s\n", new_entry->ident);
            }
        }
    }
    return copy;
}

paramlist copy_params(paramlist params)
{
    if(params == NULL) return NULL;
    paramlist copy = alcparam(params->name, params->type);
    copy->next = copy_params(params->next);
    return copy;
}

/**
 * For leaf categories other than NAME
 */
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

/**
 * Get the base type code from the token 
 * integer codes we used in lexical analysis
 */
int get_token_type_code(struct token *tok)
{
   return get_token_type(tok)->basetype;
}

/**
 * Starting point: funcdef
 *
 * We've arrived at a funcdef and need to get the symtable entry for the 
 *   function 
 * TODO: What if there's no return statment?
 * 
*/
void typecheck_func_ret_type(struct tree *t)
{   
    if(t == NULL) return;
    SymbolTableEntry fentry = NULL;
    struct token *ftok = NULL;
    switch(t->prodrule) {
        case FUNCDEF:
            // If the returntype of our function is not ANY_TYPE, we must 
            //   confirm that the specified returntype matches the actual type
            //   returned.
            ftok = t->kids[0]->leaf;
            fentry = lookup(ftok->text, t->stab);
            typeptr returntype = fentry->typ->u.f.returntype;
            if(returntype->basetype != ANY_TYPE) {
                int has_return_stmt = typecheck_func_ret_type_aux(t, returntype, ftok);
                // If the function doesn't have a return_stmt or it returns 
                //   nothing it's return type had better be None
                if(!has_return_stmt && returntype->basetype != NONE_TYPE)
                    semantic_error(ftok, "'%s()' return type '%s' does not match type of value returned: '%s'\n", ftok->text, print_type(returntype), print_type(none_typeptr));
            }
            break;
        default: {                  
            for(int i = 0; i < t->nkids; i++) {
                typecheck_func_ret_type(t->kids[i]);
            }
        }
    }
}

/**
 * Return true if we find a return_stmt. false otherwise
 */
bool typecheck_func_ret_type_aux(struct tree *t, typeptr returntype, struct token *ftok)
{
    if(t == NULL || returntype == NULL || ftok == NULL) return false;
    bool ret = false;
    switch(t->prodrule) {
        case RETURN_STMT:
            ret = true;
            typeptr ret_val = typecheck_testlist(t->kids[1]);

            // If nothing returns
            if(ret_val == NULL) ret_val = none_typeptr;
            // Grab the parent function
            int compatible = are_types_compatible(returntype, ret_val);
            if(!compatible)
                semantic_error(ftok, "'%s()' return type '%s' does not match type of value returned: '%s'\n", ftok->text, print_type(returntype), print_type(ret_val));
            break;
        default:
            for(int i = 0; i < t->nkids; i++) {
                ret = typecheck_func_ret_type_aux(t->kids[i], returntype, ftok);
                if(ret) break;
            }
    }
    return ret;
}

struct token *get_func_ancestor(struct tree *t) {
    // Since we're starting from a return statement and we've already 
    //   checked that all return statements have functions as parents,
    //   t should never be NULL
    if(t == NULL) return NULL;
    switch(t->prodrule) {
        case FUNCDEF: {
            return t->kids[0]->leaf;
        }
        default: {
            return get_func_ancestor(t->parent);
        }
    }
}

/**
 * This can be used to check types for returns and assignments/decls.
 * In the context of function returns, the returntype is the lhs, since 
 * assignments with a float on the LHS are allowed to be assigned integers.
 * Similarly, functions that specify a float return type can return explicit
 * integers. If type ANY is involved, then return true
*/
int are_types_compatible(typeptr lhs, typeptr rhs)
{
    // If either of the types are NULL, return false
    if(lhs == NULL || rhs == NULL) return 0;

    // If the rhs has ANY_TYPE, return 1
    if(rhs->basetype == ANY_TYPE) return 1;

    // If the function returntype is ANY_TYPE then anything is allowed to 
    //   be returned
    switch(lhs->basetype) {
        case ANY_TYPE: {
            return 1;
        }
        case CLASS_TYPE: {
            // If the LHS is a class type, ensure that the RHS is a  
            //   class with the same name
            if(rhs->basetype != CLASS_TYPE) {
                return 0;
            }
            else {
                if(rhs->basetype != CLASS_TYPE) return 0;
                if(lhs->u.cls.name == NULL || rhs->u.cls.name == NULL) return 0;
                    return 0;
                if(strcmp(lhs->u.cls.name, rhs->u.cls.name) == 0) {
                    return 1;
                }
                return 0;
            }
        }
        case FLOAT_TYPE: {
            // If the function's specified returntype is a FLOAT return true 
            // if the function actually returns ANY_TYPE, INT_TYPE, or 
            // FLOAT_TYPE.
            switch(rhs->basetype) {
                case FLOAT_TYPE:
                case INT_TYPE:
                    return 1;
            }
            return 0;
        }
        default: {
            if(lhs->basetype == rhs->basetype) 
                return 1;
        }
    }
    return 0;
}


/**
 * BASETYPE -> "any"
*/
const char *print_type(typeptr type)
{
    if(type == NULL) return "NULL";
    switch(type->basetype) {
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
        case FILE_TYPE: 
            return "file";
        default:
            // This usually means a type was not initialized correctly
            return "mystery";
    }
}

void print_paramlist(paramlist params) 
{
    if(params == NULL) return;
    if(params->name != NULL && params->type != NULL) 
        printf("%s: %s, ", params->name, print_type(params->type));
    print_paramlist(params->next);
}

/**
 * Get the rightmost right-hand side of an assignment expression. It should 
 * return a basetype integer code. Our assumed starting point is
 * testlist. If multiple consecutive assignments are found we need to verify 
 * that these are also assigned correct types
 */
struct typeinfo *get_rhs_type(struct tree *t)
{
    if(t == NULL) return alcbuiltin(ANY_TYPE);
    struct typeinfo *type = NULL;

    // Recurse until the "power" nonterminal is found
    switch(t->prodrule) {
        case POWER: { 
            type = typecheck_power(t);
            break;
        } 

        case NAME: {
            type = get_ident_type(t->leaf->text, t->stab);
            break;
        }

        // If we see listmaker_opt, we know that it's a list (e.g., [1, 2, b])
        case LISTMAKER_OPT: {
            // Ensure that the contents of the list are legal
            typecheck_listmaker_contents(t);
            type = list_typeptr;
            break;
        }

        // Dictionary
        case DICTORSETMAKER_OPT: {
            /* Right-hand side is a dictionary */
            typecheck_dictmaker_contents(t);
            type = dict_typeptr;
            break;
        }
        default: {
            // It is assumed that we can just recurse the first child until one of 
            //   the above three options is found 
            type = get_rhs_type(t->kids[0]);
        }
    }
    if(type == NULL)
        return alcbuiltin(ANY_TYPE);
    return type;
}
