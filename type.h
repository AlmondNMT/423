#ifndef TYPE_H
#define TYPE_H
#include "errdef.h"

struct tree;
struct token;
struct sym_entry;

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
#define USER_DEF     1000012 // Use this for user-defined class instances

#define LAST_TYPE    1000013

typedef struct typeinfo {
    int basetype;
    union {
        struct funcinfo {
             char *name; /* ? */
             struct sym_table *st;
             int nparams;
             struct param *parameters;
             struct typeinfo *returntype;
        } f;
        struct classinfo {
            char *name;
            struct sym_table *st;
            int nparams;                // For constructor
            struct param *parameters;   // Constructor params
            int instance;               // Is it an instance of the class?
        } cls;
        struct packinfo {
            char *name;
            struct sym_table *st;
        } p;
    } u;
} *typeptr;

/* add constructors for other types as needed */
typeptr alctype(int basetype);
void free_typeptr(typeptr typ);
paramlist alcparam(char *name, int basetype);
void free_params(paramlist params);
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

// Prototypes for arithmetical type-checking
int type_str_to_int(char *typestr);
char *type_for_bin_op(char *lhs, char *rhs, char* op);
char *type_for_bin_op_plus(char *lhs, char *rhs);
char *type_for_bin_op_minus(char *lhs, char *rhs);
char *type_for_bin_op_times(char *lhs, char *rhs);
char *type_for_bin_op_div(char *lhs, char *rhs);
char *type_for_bin_op_equals(char *lhs, char *rhs);
char *type_for_bin_op_great_less(char *lhs, char *rhs);
char *type_for_bin_op_logical(char *lhs, char *rhs);

// Adding type info 
void add_type_info(struct tree *t, struct sym_table *st);
void add_decl_type_info(struct tree *t, struct sym_table *st);
void add_class_type_info(struct tree *t, struct sym_table *st);
void add_expr_type_info(struct tree *t, struct sym_table *st);

// Type-checking
void type_check(struct tree *t, struct sym_table *st);

void validate_operand_types(struct tree *t, struct sym_table *st);
void validate_or_test(struct tree *t, struct sym_table *st);
void verify_func_ret_type(struct tree *t, struct sym_table *st);
void verify_func_arg_types(struct tree *t, struct sym_table *st);
void verify_decl_types(struct tree *t, struct sym_table *st);

struct token *get_func_ancestor(struct tree *t);
struct token *get_caller_ancestor(struct tree *t);
int are_types_compatible(typeptr lhs, typeptr rhs);
struct typeinfo* get_fpdef_type(struct tree *t, struct sym_table * ftable);
struct typeinfo *get_rhs_type(struct tree *t);
struct typeinfo *get_arglist_opt_type(struct tree *t, struct sym_table * st, struct sym_entry *entry);
void check_var_type(struct typeinfo *lhs_type, struct typeinfo *rhs_type, struct token *tok);
struct typeinfo *get_trailer_type(struct tree *t, struct sym_table * st, struct sym_entry *entry);
struct typeinfo *get_trailer_type_list(struct tree *t, struct sym_table * st);
struct typeinfo *get_type_of_node(struct tree *t, struct sym_table * st, struct sym_entry *entry);
void handle_or_test_types(struct tree *t, struct sym_table * st);

// Type annotations
void get_function_params(struct tree *t, struct sym_table *ftable);
void add_nested_table(struct sym_entry *, struct typeinfo *rhs_type);

// ex: "int" -> INT_TYPE
const char* get_basetype(int basetype);

// ex: "name" -> typeinfo*. Returns an allocated typeinfo pointer
struct typeinfo *get_ident_type(char *ident, struct sym_table * st);

// identifier type code
int get_ident_type_code(char *ident, struct sym_table * st);
struct typeinfo *get_token_type(struct token *tok);
struct typeinfo *determine_hint_type(struct token *type, struct sym_table * st);

// Factory type copier
struct typeinfo *type_copy(struct typeinfo *typ);

// Copy functions
struct sym_table *copy_symbol_table(struct sym_table *);
paramlist copy_params(paramlist params);

// Add builtins
struct sym_entry *insertbuiltin(struct sym_table * global, char *s, int basetype);
struct sym_entry *insertbuiltin_meth(struct sym_table *builtin_table, char *name, char *ret_type);

// Print params
void print_paramlist(paramlist params);
#endif
