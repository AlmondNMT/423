#ifndef TYPE_H
#define TYPE_H
#include <stdbool.h>
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
#define LIST_TYPE    1000003
#define FLOAT_TYPE   1000004
#define DICT_TYPE    1000006
#define BOOL_TYPE    1000007
#define STRING_TYPE  1000008
#define FILE_TYPE    1000011

#define CLASS_TYPE   1000002
#define FUNC_TYPE    1000005
#define PACKAGE_TYPE 1000009
#define ANY_TYPE     1000010
#define USER_DEF     1000012 // Use this for user-defined class instances

#define LAST_TYPE    1000013

typedef struct typeinfo {
    int basetype;
    union {
        struct funcinfo {
             char *name; /* ? */
             struct sym_table *st;
             int nparams;
             int max_params;
             int min_params;
             bool vararg;
             struct param *parameters;
             struct typeinfo *returntype;
        } f;
        struct classinfo {
            char *name;
            struct sym_table *st;
            int nparams;                // For constructor
            int max_params;
            int min_params;
            bool vararg;
            struct param *parameters;   // Constructor params
            struct typeinfo *returntype;
            int instance;               // Is it an instance of the class?
        } cls;
        struct packinfo {
            char *name;
            struct sym_table *st;
        } p;
    } u;
} *typeptr;


//
struct trailer {
    int prodrule;
    char *name;
    struct trailer *next;
    struct arg *arg;        // Can be used for the arguments to lists or functions
};

struct arg {
    struct arg *next; // Next argument in sequence of arguments
    typeptr type;     // The type of the argument
};


struct trailer *create_trailer_link(char *name, int prodrule);
struct trailer *build_trailer_sequence(struct tree *t);
struct arg *build_arglist(struct tree *t);
struct arg *create_arg_link(typeptr type);
void free_trailer_sequence(struct trailer *seq);
void free_arglist(struct arg *args);
void print_trailer_sequence(struct trailer *seq);
void print_arglist(struct arg *args);


/* add constructors for other types as needed */
void init_types();
typeptr alctype(int basetype);
void free_typeptr(typeptr typ);
paramlist alcparam(char *name, typeptr type);
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
typeptr type_for_bin_op(typeptr lhs, typeptr rhs, struct token *tok);
typeptr type_for_bin_op_plus(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_minus(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_times(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_div(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_equals(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_great_less(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_logical(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_bitwise(typeptr lhs, typeptr rhs);
typeptr type_for_bin_op_mod(typeptr lhs, typeptr rhs);

// Type-checking
void typecheck_expr_stmt(struct tree *t);
void typecheck(struct tree *t);
void typecheck_decl_stmt(struct tree *t);
void typecheck_func_ret_type(struct tree *t);
bool typecheck_func_ret_type_aux(struct tree *t, typeptr returntype, struct token *ftok);
void typecheck_listmaker_contents(struct tree *t);
typeptr typecheck_atom_trailer(struct trailer *seq, typeptr atom_type, struct token *desc);
struct typeinfo *typecheck_testlist(struct tree *t);
struct typeinfo *typecheck_power(struct tree *t);
struct typeinfo *typecheck_factor(struct tree *t);
struct typeinfo *typecheck_op(struct tree *t);
struct typeinfo *typecheck_op_aux(struct tree *t, typeptr lhs);

void validate_operand_types(struct tree *t, struct sym_table *st);
void validate_or_test(struct tree *t, struct sym_table *st);
void validate_subscript_usage(typeptr current_type, struct trailer *curr, struct token *tok);

void check_forbidden_list_and_dict_types(struct tree *t, typeptr type, struct token *desc);

// Correct function usage
int count_args(struct arg *args);
void check_args_with_params(struct arg *args, struct param *params, struct token *tok, int count);
void validate_args_and_params(struct arg *args, struct sym_entry *rhs, struct token *tok);


void verify_func_arg_count(struct tree *t);
void verify_correct_func_use(struct tree *t, struct sym_table *st);
void verify_func_arg_types(struct tree *t, struct sym_table *st);
void verify_decl_types(struct tree *t, struct sym_table *st);
void disallow_funccall_no_parenth(struct tree *t);
void disallow_funccall_no_parenth_aux(struct tree *t);

struct token *get_func_ancestor(struct tree *t);
struct token *get_caller_ancestor(struct tree *t);
struct token *get_power_ancestor(struct tree *t);
struct token *get_power_descendant(struct tree *t);
bool are_types_compatible(typeptr lhs, typeptr rhs);
struct typeinfo *get_fpdef_type(struct tree *t, struct sym_entry *entry);
struct typeinfo *get_rhs_type(struct tree *t);
struct typeinfo *get_trailer_rep_type(struct trailer *seq, struct sym_entry *entry, struct token *tok);

// Type annotations
void get_function_params(struct tree *t, struct sym_entry *entry);
void add_nested_table(struct sym_entry *, struct typeinfo *rhs_type);
void add_param_to_function_entry(struct token *param_name_tok, typeptr param_type, struct sym_entry *fentry);

// ex: type -> "int"
const char* print_type(typeptr type);

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

// Print params
void print_paramlist(paramlist params);
#endif
