#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nonterminal.h"
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"
#include "utils.h"
 
struct sym_table;
struct sym_entry;

extern struct sym_table *mknested(char *, int, int, struct sym_table *, char *);
extern struct sym_entry *insertsymbol(struct sym_table *, struct token *);

char *typenam[] =
   {"none", "int", "class", "list", "float", "func", "dict", "bool",
    "string", "package", "any"};

int calc_nparams(struct tree *t) {
    return 0;
}

/*typeptr alcstructtype()
{
   typeptr rv = alctype(STRUCT_TYPE);
   // who initializes the fields, someone else I guess, later on 
   return rv;
}*/

/* in order for this to make any sense, you have to pass in the subtrees
 * for the return type (r) and the parameter list (p), but the calls to
 * to this function in the example are just passing NULL at present!
 */
typeptr alcfunctype(struct tree * r, struct tree * p, SymbolTable st)
{
   typeptr rv = alctype(FUNC_TYPE);
   rv->u.f.st = st;
   /* fill in return type and paramlist by traversing subtrees */
   /* rf->u.f.returntype = ... */
   return rv;
}

typeptr alcclasstype(struct tree *r, struct tree *p, SymbolTable st)
{
    typeptr rv = alctype(CLASS_TYPE);
    rv->u.cls.st = st;
    return rv;
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
char *type_for_bin_op(char *lhs, char *rhs, char* op)
{

    char* ret = "ERROR"; //default error

    if(strcmp(op, "+") == 0)
    {
        ret = type_for_bin_op_plus(rhs, lhs);
    }

    if(strcmp(op, "-") == 0)
    {
        ret = type_for_bin_op_minus(rhs, lhs);
    }

    if(strcmp(op, "*") == 0)
    {
        ret = type_for_bin_op_times(rhs, lhs);
    }

    if(strcmp(op, "/") == 0)
    {
        ret = type_for_bin_op_div(rhs, lhs);
    }    

    if(strcmp(op, ">") == 0 || strcmp(op, "<") == 0 )
    {
        ret = type_for_bin_op_great_less(rhs, lhs);
    }

    if( strcmp(op, "==") == 0 )
    {
        ret = type_for_bin_op_equals(rhs, lhs);
    }

    if( strcmp(op, "and") == 0 || strcmp(op, "or") == 0)
    {
        ret = type_for_bin_op_logical(rhs, lhs);
    }

    //all the above funcs will return ERROR if no match is found
    if(strcmp("ERROR", ret)==0){
        fprintf(stderr, "Mismatched type between %s and %s or operator %s inappllcable\n", lhs, rhs, op);
        exit(SEM_ERR);
    }
    return ret;
    //return type_str_to_int(ret);
}


//operand match check for plus
char *type_for_bin_op_plus(char *lhs, char *rhs)
{
    //if anything is Any, don't bother, just return Any anyways
    if(strcmp(lhs, "any") == 0 || strcmp(rhs, "any") == 0)
        return "any";

    //handle int plus something
    if(strcmp(lhs, "int") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "int";

    }

    //handle float plus something
    if(strcmp(lhs, "float") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "float";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "float";
    }

    //handle str plus something
    if(strcmp(lhs, "str") == 0)
    {
        if(strcmp(rhs, "str") == 0)
            return "str"; 
    }

    //handle bool plus something
    if(strcmp(lhs, "bool") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int"; 

        if(strcmp(rhs, "float") == 0)
            return "float"; 

        if(strcmp(rhs, "bool") == 0)
            return "bool"; 
    }

    //handle str plus something
    if(strcmp(lhs, "list") == 0)
    {
        if(strcmp(rhs, "list") == 0)
            return "list"; 
    }
    
    return "ERROR";
}

//operand match check for minus
char *type_for_bin_op_minus(char *lhs, char *rhs)
{
    //if anything is Any, don't bother, just return Any anyways
    if(strcmp(lhs, "any") == 0 || strcmp(rhs, "any") == 0)
        return "any";

    //handle int minus something
    if(strcmp(lhs, "int") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "int";

    }

    //handle float minus something
    if(strcmp(lhs, "float") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "float";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "float";
    }


    //handle bool minus something
    if(strcmp(lhs, "bool") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int"; 

        if(strcmp(rhs, "float") == 0)
            return "float"; 

        if(strcmp(rhs, "bool") == 0)
            return "bool"; 
    }
    
    return "ERROR";
}

//operand match check for times
char *type_for_bin_op_times(char *lhs, char *rhs)
{
    //if anything is Any, don't bother, just return Any anyways
    if(strcmp(lhs, "any") == 0 || strcmp(rhs, "any") == 0)
        return "any";

    //handle int times something
    if(strcmp(lhs, "int") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "int";

    }

    //handle float times something
    if(strcmp(lhs, "float") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "float";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "float";
    }

    //handle str times something
    //DECIDED NOT TO ALLOW THIS FOR NOW
    //BUT KEEPING THE COMMENTED VERSION
    /*
    if(strcmp(lhs, "str") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "str"; 
    }*/

    //handle bool times something
    if(strcmp(lhs, "bool") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int"; 

        if(strcmp(rhs, "float") == 0)
            return "float"; 

        if(strcmp(rhs, "bool") == 0)
            return "bool"; 
    }

    //handle list times something
    if(strcmp(lhs, "list") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "list"; 
    }
    
    return "ERROR";
}

//operand match check for times
char *type_for_bin_op_div(char *lhs, char *rhs)
{
    //if anything is Any, don't bother, just return Any anyways
    if(strcmp(lhs, "any") == 0 || strcmp(rhs, "any") == 0)
        return "any";

    //handle int by something
    if(strcmp(lhs, "int") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "int";

    }

    //handle float by something
    if(strcmp(lhs, "float") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "float";

        if(strcmp(rhs, "float") == 0)
            return "float";

        if(strcmp(rhs, "bool") == 0)
            return "float";
    }


    //handle bool by something
    if(strcmp(lhs, "bool") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "int"; 

        if(strcmp(rhs, "float") == 0)
            return "float"; 

        if(strcmp(rhs, "bool") == 0)
            return "bool"; 
    }
    
    return "ERROR";
}

//operand match check for ==, will always return bool because python is amazing
char *type_for_bin_op_equals(char *lhs, char *rhs)
{
        return "bool";
}

//operand match check for greater and less kind of operators
char *type_for_bin_op_great_less(char *lhs, char *rhs)
{
    //if anything is Any, don't bother, just return Any anyways
    if(strcmp(lhs, "any") == 0 || strcmp(rhs, "any") == 0)
        return "any";

    //handle int grless something
    if(strcmp(lhs, "int") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "bool";

        if(strcmp(rhs, "float") == 0)
            return "bool";

        if(strcmp(rhs, "bool") == 0)
            return "bool";

    }

    //handle float grless something
    if(strcmp(lhs, "float") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "bool";

        if(strcmp(rhs, "float") == 0)
            return "bool";

        if(strcmp(rhs, "bool") == 0)
            return "bool";
    }

    //handle str times something
    if(strcmp(lhs, "str") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "bool"; 

        if(strcmp(rhs, "str") == 0)
            return "bool"; 
    }

    //handle bool grless something
    if(strcmp(lhs, "bool") == 0)
    {
        if(strcmp(rhs, "int") == 0)
            return "bool"; 

        if(strcmp(rhs, "float") == 0)
            return "bool"; 

        if(strcmp(rhs, "bool") == 0)
            return "bool"; 
    }

    //handle list grless something
    if(strcmp(lhs, "list") == 0)
    {
        if(strcmp(rhs, "list") == 0)
            return "bool"; 
    }
    
    return "ERROR";
}

//operand match check for logical operators
//as in python these behave totally different from normal 
//logical operators and types also depend on values
//this just returns any if its not expressly two bools
char *type_for_bin_op_logical(char *lhs, char *rhs)
{       
        //if both are bools, return a goddamn bool
        if(strcmp(rhs,"bool") == 0 && strcmp(lhs,"bool") == 0)
            return "bool";

        return "any";
}

// builtin type allocations
typeptr alctype(int basetype)
{
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

typeptr alcfunc(char *name, int nparams, int pbasetype)
{
    typeptr ptr = alctype(FUNC_TYPE);
    ptr->u.f.name = strdup(name);
    ptr->u.f.nparams = nparams;
    ptr->u.f.parameters = alcparam("p", pbasetype);
    return ptr;
}

typeptr alcbuiltin(int basetype)
{
    typeptr ptr = alctype(basetype);
    ptr->u.cls.nparams = 1;

    // The constructor param is ANY_TYPE
    ptr->u.cls.parameters = alcparam("p", ANY_TYPE);
    return ptr;
}

typeptr alclist()
{
    typeptr list = alctype(LIST_TYPE);
    struct sym_table *st = mksymtab(HASH_TABLE_SIZE, "class");
    list->u.cls.name = strdup("list");
    list->u.cls.st = st;
    insertbuiltin_meth(st, "append", "None");
    insertbuiltin_meth(st, "remove", "None");
    return list;
}

/**
 * Add methods to a builtin class
 */
struct sym_entry *insertbuiltin_meth(struct sym_table *btable, char *name, char *ret_type)
{
    struct sym_entry *entry = NULL;
    struct token *tok = create_builtin_token(name);
    entry = insertsymbol(btable, tok);
    entry->typ->basetype = FUNC_TYPE;
    entry->typ->u.f.returntype = get_ident_type(ret_type, NULL); // This should be fine
    return entry;
}

typeptr alcdict()
{
    typeptr dict = alctype(DICT_TYPE);
    struct sym_table *st = mksymtab(HASH_TABLE_SIZE, "class");
    dict->u.cls.name = strdup("dict");
    dict->u.cls.st = st;
    insertbuiltin_meth(st, "keys", "list");
    insertbuiltin_meth(st, "values", "list");
    return dict;
}


typeptr alcfile()
{
    typeptr file = (typeptr) alctype(FILE_TYPE);
    struct sym_table *st = mksymtab(HASH_TABLE_SIZE, "class");
    file->u.cls.name = strdup("file");
    file->u.cls.st = st;
    insertbuiltin_meth(st, "write", "int");
    insertbuiltin_meth(st, "close", "None");
    insertbuiltin_meth(st, "read", "int");
    return file;
}

typeptr alcstr()
{
    typeptr str = (typeptr) alctype(STRING_TYPE);
    struct sym_table *st = mksymtab(HASH_TABLE_SIZE, "class)");
    str->u.cls.name = strdup("str");
    str->u.cls.st = st;
    insertbuiltin_meth(st, "replace", "str");
    insertbuiltin_meth(st, "split", "list");
    return str;
}

typeptr alcnone()
{
    typeptr none = (typeptr) alctype(NONE_TYPE);
    none->u.cls.name = strdup("None");
    return none;
}

paramlist alcparam(char *name, int basetype)
{
    paramlist params = ckalloc(1, sizeof(struct param));
    params->name = strdup(name);
    params->type = ckalloc(1, sizeof(struct typeinfo));
    params->type->basetype = basetype;
    params->next = NULL;
    return params;
}

/**
 * Starting position: root
 * 
*/
void type_check(struct tree *t, SymbolTable st)
{
    // Verify that the return type in a function matches the stated 
    //   return type, if applicable
    verify_func_ret_type(t, st);

    // Verify the function argument types
    verify_func_arg_types(t, st);

    // Verify declarations with RHS assignments
    verify_decl_types(t, st);

    // Ensure that operand types are valid for arithmetic and logical expressions
    validate_operand_types(t, st);
}

/**
 * DECL type checking for decl+initialization
*/
void verify_decl_types(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return;
    switch(t->prodrule) {
        case DECL_STMT: {
            if(t->kids[2]->prodrule == EQUAL_TEST_OPT) {
                typeptr assignment_type = NULL, decl_type = NULL;

                // Pass the second child of the equal_test_opt, cuz the 
                //   first child is '='
                assignment_type = get_rhs_type(t->kids[2]->kids[1]);
                //decl_type = get_rhs_type()
            }
            break;
        }
        default: {
            for(int i = 0; i < t->nkids; i++) {
                verify_decl_types(t->kids[i], st);
            }
        }
    }
}

/**
 * Ensure that the types of the arguments in the function/constructor call match
 *   those in the description
*/
void verify_func_arg_types(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return;
    switch(t->prodrule) {
        case ARGLIST_OPT: {
            struct token *ftok = get_caller_ancestor(t);

            // If we were actually able to find a caller ancestor token
            if(ftok != NULL)  {
                SymbolTableEntry fentry = lookup(ftok->text, t->stab);
                if(fentry != NULL && fentry->typ != NULL) {
                    // TODO: Verify params and args
                }
            }
            break;
        }
        default: {
            for(int i = 0; i < t->nkids; i++) {
                verify_func_arg_types(t->kids[i], st);
            }
        }
    }
}

/**
 * 
*/
struct token *get_caller_ancestor(struct tree *t)
{
    if(t == NULL) return NULL;
    switch(t->prodrule) {
    }
}

void free_params(paramlist params)
{
    if(params == NULL)
        return;
    free_typeptr(params->type);
    free_params(params->next);
    free(params);
}

void free_typeptr(typeptr typ)
{
    if(typ == NULL) return;
    if(typ->basetype == FUNC_TYPE) {
        free_typeptr(typ->u.f.returntype);
        free_symtab(typ->u.cls.st);
        free_params(typ->u.f.parameters);
    } 
    else if(typ->basetype == CLASS_TYPE || typ->basetype == USER_DEF) {
        free_params(typ->u.cls.parameters);
        free_symtab(typ->u.cls.st);
    } 
    else if(typ->basetype == PACKAGE_TYPE) {
        free_symtab(typ->u.p.st);
    }
    free(typ);
}
/**
 * Another tree traversal for adding type information
 * Starting position: root
 * Assumptions: The symbol table should be fully populated
 * TODO:
 *  ☑ Finish adding function type information
 *  ☐ Add class type information (if you feel like it)
 *  ☐ Propagate type information through assignments 
 *  ☐ Get type information for declarations
*/
void add_type_info(struct tree *t, SymbolTable st)
{
    switch(t->prodrule) {
        case CLASSDEF:
            add_class_type_info(t, st);
            return;
        case EXPR_STMT:
            add_expr_type_info(t, st);
            return;
        case DECL_STMT: {
            add_decl_type_info(t, st);
            return;
        }
    }
    for(int i = 0; i < t->nkids; i++) {
        add_type_info(t->kids[i], st);
    }
}


void add_class_type_info(struct tree *t, SymbolTable st)
{

}

void add_expr_type_info(struct tree *t, SymbolTable st)
{

}

/**
 * DECL_STMT
*/
void add_decl_type_info(struct tree *t, SymbolTable st)
{
    if(t == NULL || st == NULL) return;
    SymbolTableEntry lhs = lookup(t->kids[0]->leaf->text, t->stab);

    // Free the default ANY_TYPE 
    free_typeptr(lhs->typ);
    lhs->typ = get_ident_type(t->kids[1]->leaf->text, t->stab);
}

/**
 * Get the function param type hint
 */
struct typeinfo *get_fpdef_type(struct tree *t, SymbolTable ftable)
{
    struct typeinfo *ret = NULL;
    if(t == NULL || ftable == NULL) // This probably should never happen
        return alcbuiltin(ANY_TYPE);
    if(t->prodrule == POWER) {
        ret = determine_hint_type(t->kids[0]->leaf, ftable);
        t->kids[0]->type = ret;
        return ret;
    } else {
        ret = get_fpdef_type(t->kids[0], ftable);
    }
    return ret;
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

    // The fpdef nonterminal contains information about the parameter 
    //   and its type, including its name
    if(t->prodrule == FPDEF) {
        if(t->kids[1]->prodrule == COLON_TEST_OPT) {
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
        SymbolTableEntry entry = insertsymbol(ftable, leaf);
        free_typeptr(entry->typ);
        entry->typ = type;
    } 
    else {
        for(int i = 0; i < t->nkids; i++) {
            get_function_params(t->kids[i], ftable);
        }
    }
    decorate_subtree_with_symbol_table(t, ftable);
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
    if(t->kids[1]->prodrule == NULLTREE)
    {
        return;
    }
    else 
    {
        lhs_type = get_rhs_type(t);
        rhs_type = get_rhs_type(t->kids[1]);
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
    else if(strcmp(ident, "None") == 0)
        return alcnone();
    else {
        //here we know that it is NOT a builtin
        //so we look up the entry of said name
        //in an inside to outside direction (which is what lookup does)
        SymbolTableEntry type_entry = lookup(ident, st);
        if(type_entry != NULL) {
            //get type info if entry found
            return type_copy(type_entry->typ);
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
 * Copy the typeptr of custom user classes
 * 
*/
struct typeinfo *type_copy(struct typeinfo *typ)
{
    // End of recursion
    if(typ == NULL) {
        return NULL;
    }

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
            if(old_entry->nested != NULL)
                new_entry->nested = copy_symbol_table(old_entry->nested);
        }
    }
    return copy;
}

paramlist copy_params(paramlist params)
{
    if(params == NULL) return NULL;
    paramlist copy = alcparam(params->name, params->type->basetype);
    copy->next = copy_params(params->next);
    return copy;
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

/**
 * Get the base type code from the token 
 * integer codes we used in lexical analysis
 */
int get_token_type_code(struct token *tok)
{
   return get_token_type(tok)->basetype;
}

/**
 * Traverse the whole tree and verify that each function's returntype matches 
 * the type of the actual returned value. 
 * 
*/
void verify_func_ret_type(struct tree *t, SymbolTable st)
{   
    if(t == NULL || st == NULL) return;
    switch(t->prodrule) {
        case RETURN_STMT: {
            // If we find a return stmt, we need to confirm that the return types match
            typeptr ret_val = get_rhs_type(t->kids[0]);
            // Grab the parent function
            struct token *ftok = get_func_ancestor(t);
            SymbolTableEntry fentry = lookup(ftok->text, t->stab);
            int compatible = are_types_compatible(ret_val, fentry->typ->u.f.returntype);
            if(!compatible)
                semantic_error(ftok->filename, ftok->lineno, "'%s()' return type does not match type of value returned\n", ftok->text);
            break;
        }
        default: {                  
            for(int i = 0; i < t->nkids; i++) {
                verify_func_ret_type(t->kids[i], st);
            }
        }
    }
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
 * This can be used to check types for returns and assignments/decls
*/
int are_types_compatible(typeptr lhs, typeptr rhs)
{
    // If either of the types are NULL, return false
    if(lhs == NULL || rhs == NULL) return 0;

    // If the function returntype is ANY_TYPE then anything is allowed to 
    //   be returned
    // If the function 
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
        default: {
            if(lhs->basetype == rhs->basetype) 
                return 1;
        }
    }
    return 0;
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

/**
 * BASETYPE -> "any"
*/
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
            // This usually means a type was not initialized correctly
            return "mystery";
    }
}

void print_paramlist(paramlist params) 
{
    if(params == NULL) return;
    if(params->name != NULL && params->type != NULL) 
        printf("%s: %s, ", params->name, get_basetype(params->type->basetype));
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

            // If we see an ATOM then we must traverse further to get the 
            //   type 
            if(t->kids[0]->prodrule == ATOM) {
                return get_rhs_type(t->kids[0]);
            }
            if(t->kids[0]->leaf != NULL) {
                // 'power' can only be reached if we haven't already recursed to a 
                //   listmaker or a dictorset_option_* tree node 
                // This leaf contains the leftmost name of the expr_stmt
                struct token *leaf = t->kids[0]->leaf;

                // If the RHS token is a name
                if(leaf->category == NAME) {
                    
                    SymbolTableEntry entry = lookup(leaf->text, t->stab);

                    // Throw an error if entry could not be found
                    if(entry == NULL) {
                        undeclared_error(leaf);
                    }

                    // Forget about trailer_reps here, just get the identifier type
                    type = get_ident_type(entry->ident, t->stab);
                }

                // If the RHS token is a literal, dict, list, bool, or None
                else {
                    type = get_token_type(leaf);
                }
            }
            break;
        } 

        // If we see listmaker_opt, we know that it's a list (e.g., [1, 2, b])
        case LISTMAKER_OPT: {
            type = alclist();
            break;
        }

        // Dictionary
        case DICTORSETMAKER_OPT: {
            /* Right-hand side is a dictionary */
            type = alcdict();
            break;
        }
        default: {
            /* It is assumed that we can just recurse the first child until one of 
            * the above three options is found 
            * TODO: Fix bad assumption */
            type = get_rhs_type(t->kids[0]);
        }
    }
    if(type == NULL)
        return alcbuiltin(ANY_TYPE);
    return type;
}