#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "symtab.h"
#include "tree.h"
#include "punygram.tab.h"
   
struct typeinfo none_type = { NONE_TYPE };
struct typeinfo integer_type = { INT_TYPE };
struct typeinfo class_type = { CLASS_TYPE };
struct typeinfo list_type = { LIST_TYPE };
struct typeinfo float_type = { FLOAT_TYPE };
struct typeinfo func_type = { FUNC_TYPE };
struct typeinfo dict_type = { DICT_TYPE };
struct typeinfo bool_type = { BOOL_TYPE };
struct typeinfo string_type = { STRING_TYPE };
struct typeinfo package_type = { PACKAGE_TYPE };
struct typeinfo any_type = { ANY_TYPE };

typeptr none_typeptr = &none_type;
typeptr integer_typeptr = &integer_type;
typeptr class_typeptr = &class_type;
typeptr list_typeptr = &list_type;
typeptr float_typeptr = &float_type;
typeptr func_typeptr = &func_type;
typeptr dict_typeptr = &dict_type;
typeptr bool_typeptr = &bool_type;
typeptr string_typeptr = &string_type;
typeptr package_typeptr = &package_type;
typeptr any_typeptr = &any_type;

char *typenam[] =
   {"none", "int", "class", "list", "float", "func", "dict", "bool",
    "string", "package", "any"};

typeptr alctype(int base)
{
    typeptr rv;
    switch(base) {
        case NONE_TYPE: return none_typeptr;
        case INT_TYPE: return integer_typeptr;
        case CLASS_TYPE: return class_typeptr;
        case LIST_TYPE: return list_typeptr;
        case FLOAT_TYPE: return float_typeptr;
        case FUNC_TYPE: return func_typeptr;
        case DICT_TYPE: return dict_typeptr;
        case BOOL_TYPE: return bool_typeptr;
        case STRING_TYPE: return string_typeptr;
        case PACKAGE_TYPE: return package_typeptr;
        case ANY_TYPE: return any_typeptr;
    }

    rv = (typeptr) calloc(1, sizeof(struct typeinfo));
    if (rv == NULL) return rv;
    rv->basetype = base;
    return rv;
}

/* mebbe list size determination from a tree nodeptr is still reasonable? */
typeptr alclist()
{
   typeptr rv = alctype(LIST_TYPE);
   return rv;
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
   if (rv == NULL) return NULL;
   rv->u.f.st = st;
   /* fill in return type and paramlist by traversing subtrees */
   /* rf->u.f.returntype = ... */
   return rv;
}

char *typename(typeptr t)
{
   if (!t) return "(NULL)";
   else if (t->basetype < FIRST_TYPE || t->basetype > LAST_TYPE)
      return "(BOGUS)";
   else return typenam[t->basetype-1000000];
}
