#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "symtab.h"
#include "tree.h"
#include "punygram.tab.h"
#include "utils.h"
 
struct sym_table;
struct sym_entry;

extern struct sym_table *mknested(char *, int, int, struct sym_table *, char *);
extern struct sym_entry *insertsymbol(struct sym_table *, char *s, int lineno, char *filename);

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
    entry = insertsymbol(btable, name, -1, "(builtins)");
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


void add_type_info(struct tree *t, SymbolTable st)
{
    // TODO: Add type information to the populated tree nodes
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

