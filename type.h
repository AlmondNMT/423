#ifndef TYPE_H
#define TYPE_H

struct tree;

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

#define LAST_TYPE    1000010

typedef struct typeinfo {
int basetype;
union {
    struct funcinfo {
         char *name; /* ? */
         int defined; /* 0 == prototype, 1 == not prototype; I don't know if this is necessary */
         struct sym_table *st;
         struct typeinfo *returntype;
         int nparams;
         struct param *parameters;
    } f;
} u;
} *typeptr;

/* add constructors for other types as needed
typeptr alctype(int);
typeptr alclist(?);*/
typeptr alcfunctype(struct tree * r, struct tree * p, struct sym_table * st);
char *typename(typeptr t);

extern struct sym_table *global_table;
extern typeptr integer_typeptr;
extern typeptr double_typeptr;
extern typeptr char_typeptr;
extern typeptr null_typeptr;
extern typeptr string_typeptr;

extern char *typenam[];

#endif
