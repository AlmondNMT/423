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
struct code * gen_stmts(struct tree *t, struct code *code);
struct code * gen_globals(struct tree *t, struct code *code);
struct code * gen_func_params(struct tree *t, struct code *code);
struct code * gen_decl_stmt(struct tree *t, struct code *code);

// Factory function for struct code linked list generation
struct code *create_code(char *fmt, ...);
void free_code(struct code *code);
void print_code(struct code *code);

struct code * append_code(struct code *head, struct code *node);

// Name mangling
char *mangle_suffix(struct token *tok);
char *concat(char *str1, char *str2);

#endif
