#ifndef TYPE_H
#define TYPE_H
#include "errdef.h"

struct tree;

// Linked list of function/constructor parameters
typedef struct param {
   char *name;
   struct typeinfo *type;
   struct param *next;
} *paramlist;

struct field {			/* members (fields) of structs */
   char *name;
   struct type *elemtype;
};

/* base types. How many more base types do we need? */
#define FIRST_TYPE   1000000

#define NONE_TYPE    1000000
#define INT_TYPE     1000001
#define CLASS_TYPE   1000002
#define LIST_TYPE    1000003
#define FLOAT_TYPE   1000004
#define FUNC_TYPE    1000005
#define DICT_TYPE    1000006
#define BOOL_TYPE    1000007
#define STRING_TYPE  1000008
#define PACKAGE_TYPE 1000009
#define ANY_TYPE     1000010
#define FILE_TYPE    1000011
#define USER_DEF     1000012 // WE MAY NEVER HAVE TO USE THIS

#define LAST_TYPE    1000013

typedef struct typeinfo {
    int basetype;
    union {
        struct funcinfo {
             char *name; /* ? */
             struct sym_table *st;
             struct typeinfo *returntype;
             int nparams;
             struct param *parameters;
        } f;
        struct classinfo {
            char *name;
            struct sym_table *st;
            int instance;               // Is it an instance of the class?
            int nparams;                // For constructor
            struct param *parameters;   // Constructor params
        } cls;
        struct packinfo {
            char *name;
            struct sym_table *st;
        } p;
    } u;
} *typeptr;

/* add constructors for other types as needed */
typeptr alctype(int basetype);
paramlist alcparam(char *name, int basetype);
typeptr alcclass(char *name);
typeptr alcfunc(char *name, int nparams, int pbasetype);
typeptr alclist();
typeptr alcdict();
typeptr alcfile();
typeptr alcstr();
typeptr alcnone();
typeptr alcbuiltin(); // For str, int, float, bool, None, any
/*typeptr alcfunctype(struct tree * r, struct tree * p, struct sym_table * st);
typeptr alcclasstype(struct tree *r, struct tree *p, struct sym_table *st);*/
int calc_nparams(struct tree *t);
char *typename(typeptr t);

extern struct sym_table *global_table;
extern typeptr integer_typeptr;
extern typeptr double_typeptr;
extern typeptr char_typeptr;
extern typeptr null_typeptr;
extern typeptr string_typeptr;

extern char *typenam[];

// Prototypes
int type_str_to_int(char *typestr);
char *type_for_bin_op(char *lhs, char *rhs, char* op);
char *type_for_bin_op_plus(char *lhs, char *rhs);
char *type_for_bin_op_minus(char *lhs, char *rhs);
char *type_for_bin_op_times(char *lhs, char *rhs);
char *type_for_bin_op_div(char *lhs, char *rhs);
char *type_for_bin_op_equals(char *lhs, char *rhs);
char *type_for_bin_op_great_less(char *lhs, char *rhs);
char *type_for_bin_op_logical(char *lhs, char *rhs);


#endif
