#ifndef _BUILTINS_H
#define _BUILTINS_H

struct sym_table; 
struct sym_entry; 
struct token;
struct typeinfo;

struct token *create_builtin_token(char *name);
struct sym_entry *insertbuiltin_meth(struct sym_table *btable, char *name, struct typeinfo *returntype, char *codestr);
struct sym_entry *insertbuiltin(struct sym_table *st, char *name, int basetype, char *codename);
void add_builtin_func_info(struct sym_entry *entry, int min, int max, struct typeinfo *rettype, char *fmt, ...);
void add_puny_builtin(struct sym_table *st);
void add_random_library(struct sym_table *st);
void add_math_library(struct sym_table *st);
#endif
