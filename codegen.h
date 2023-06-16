#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <stdbool.h>

struct tree;
struct sym_table;
struct token;

struct code {
    int prodrule;
    struct code *next;
    char *codestr;
};

// Code generation prototypes
void gencode(struct tree *t, bool do_mangle);
struct code * gen_func(struct tree *t, struct code *code);
struct code * gen_stmts(struct tree *t, struct code *code, unsigned int tablevel);
struct code * gen_globals(struct tree *t, struct code *code);
struct code * gen_decl_stmt(struct tree *t, struct code *code);
struct code * gen_return_stmt(struct tree *t, struct code *code);
struct code * gen_expr_stmt(struct tree *t, struct code *code);
void gen_table(struct tree *t, struct code *code);
void gen_testlist(struct tree *t, struct code *code);
void gen_power(struct tree *t, struct code *code);
void gen_func_params(struct tree *t, struct code *func);
void gen_atom(struct tree *t, struct code *code);
void gen_tct_code(struct tree *t, struct code *code);
void gen_op_code(struct tree *t, struct code *code);
void gen_for_bin_op(struct tree *t, struct code *code);
void gen_plus_code(struct tree *t, struct code *code);
void gen_mult_code(struct tree *t, struct code *code);

// Factory function for struct code linked list generation
struct code *create_code(char *fmt, ...);

// Get the tail of the linked list
struct code * get_tail(struct code *code);
void free_code(struct code *code);
void print_code(struct code *code);

struct code * append_code(struct code *head, struct code *node);

// Name mangling and string manipulation stuff
char *mangle_suffix(struct token *tok);
char *concat(char *str1, char *str2);
char *tab(unsigned int level);
char *gen_atom_str(struct tree *t);

#endif
