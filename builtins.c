#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"

extern struct typeinfo none_type;
extern struct typeinfo int_type;
extern struct typeinfo float_type;
extern struct typeinfo bool_type;

extern struct typeinfo list_type;
extern struct typeinfo dict_type;
extern struct typeinfo string_type;
extern struct typeinfo file_type;

struct token *create_builtin_token(char *name)
{
    struct token *tok = create_token(name, "(builtins)", -1, -1);
    return tok;
}


/**
 * Add methods to a builtin class
 */
struct sym_entry *insertbuiltin_meth(struct sym_table *btable, char *name, typeptr returntype)
{
    struct sym_entry *entry = NULL;
    struct token *tok = create_builtin_token(name);
    entry = insertsymbol(btable, tok);
    free_token(tok);
    entry->typ->basetype = FUNC_TYPE;
    entry->typ->u.f.returntype = returntype;
    entry->typ->u.f.name = name;
    entry->isbuiltin = true;
    return entry;
}


/** 
 * Wrapper function for insertsymbol that adds builtins
 */
SymbolTableEntry insertbuiltin(SymbolTable st, char *name, int basetype)
{
    if(st == NULL) return NULL;
    struct token *tok = create_builtin_token(name);
    SymbolTableEntry entry = insertsymbol(st, tok);
    entry->typ->basetype = basetype;

    entry->typ->u.f.name = name;

    // Assumption: Builtins are only ever FUNCTIONS OR CLASSES
    if(basetype == CLASS_TYPE) {
        entry->typ->u.cls.name = name;
        typeptr type = get_ident_type(name, NULL);
        entry->typ->u.cls.returntype = type;
        if(type->u.cls.st != NULL) {
            entry->nested = type->u.cls.st;
            entry->nested->parent = st;
            entry->nested->level = st->level + 1;
        }
    }

    entry->isbuiltin = true;
    free_token(tok);
    return entry;
}


/**
 * 1. Add param count
 * 2. Add param types
 * 3. Add return type
*/
void add_builtin_func_info(struct sym_entry *entry, int min, int max, struct typeinfo *returntype, char *fmt, ...)
{
    // Add the function return type
    if(entry->typ == NULL)
        return;
    if(entry->typ->basetype == CLASS_TYPE) {
        entry->typ->u.cls.returntype = returntype;
        entry->typ->u.cls.min_params = min;
        entry->typ->u.cls.max_params = max;
    }
    else {
        entry->typ->u.f.returntype = returntype;
        entry->typ->u.f.min_params = min;
        entry->typ->u.f.max_params = max;
    }
    if(fmt == NULL) return;
    
    va_list args;
    va_start(args, fmt);
    paramlist params = NULL;
    char *name = NULL;
    // Search the format string for parameter names and their corresponding types
    while(*fmt != '\0') {
        switch(*fmt) {
            case '%': {
                switch(*(++fmt)) {
                    case 's': {
                        name = va_arg(args, char*);
                        while(*(++fmt) == ':' || *fmt == ' ' || *fmt == '%') fmt++;
                    }
                    case 'd': {
                        int basetype = va_arg(args, int);
                        if(params == NULL && name != NULL) {
                            params = alcparam(name, alcbuiltin(basetype));
                            entry->typ->u.f.parameters = params;
                        }
                        else if(params != NULL && name != NULL) {
                            params->next = alcparam(name, alcbuiltin(basetype));
                            params = params->next;
                        }
                        break;
                    }
                }
            }
        }
        ++fmt;
    }
    va_end(args);
}

/**
 * Manually add every builtin and its associated fields
 */
void add_puny_builtins(SymbolTable st) {
    init_types();
    SymbolTableEntry entry = NULL;

    entry = insertbuiltin(st, "any", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, alctype(ANY_TYPE), "%s: %d", "s", ANY_TYPE);

    entry = insertbuiltin(st, "print", FUNC_TYPE);
    add_builtin_func_info(entry, 0, -1, &none_type, "%s: %d", "s", ANY_TYPE);

    entry = insertbuiltin(st, "None", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &none_type, "%s: %d", "s", ANY_TYPE);

    // Abs value function takes int/float and outputs int/float (any and any)
    entry = insertbuiltin(st, "abs", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 1, alctype(ANY_TYPE), "%s: %d", "n", ANY_TYPE);

    entry = insertbuiltin(st, "bool", CLASS_TYPE);  
    entry->typ->u.cls.returntype = &bool_type; 

    entry = insertbuiltin(st, "chr", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 1, string_typeptr, "%s: %d ", "n", INT_TYPE);

    entry = insertbuiltin(st, "float", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &float_type, "%s: %d", "n", ANY_TYPE);

    entry = insertbuiltin(st, "input", FUNC_TYPE);
    add_builtin_func_info(entry, 0, 1, alctype(ANY_TYPE), "%s: %d", "s", STRING_TYPE);

    entry = insertbuiltin(st, "int", CLASS_TYPE);    
    add_builtin_func_info(entry, 0, 1, &int_type, "%s: %d", "a", INT_TYPE);

    entry = insertbuiltin(st, "len", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 1, &int_type, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "max", FUNC_TYPE);
    add_builtin_func_info(entry, 1, -1, &int_type, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "min", FUNC_TYPE);
    add_builtin_func_info(entry, 1, -1, &int_type, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "open", FUNC_TYPE);
    add_builtin_func_info(entry, 2, 2, &file_type, "%s: %d, %s: %d", "pathname", STRING_TYPE, "mode", STRING_TYPE);

    entry = insertbuiltin(st, "ord", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 1, &int_type, "%s: %d", "s", STRING_TYPE);

    entry = insertbuiltin(st, "pow", FUNC_TYPE);
    add_builtin_func_info(entry, 2, 2, alctype(ANY_TYPE), "%s: %d, %s: %d", "b", ANY_TYPE, "e", ANY_TYPE);

    entry = insertbuiltin(st, "range", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 3, &list_type, "%s: %d, %s: %d, %s: %d", "beg", INT_TYPE, "end", INT_TYPE, "step", INT_TYPE);

    entry = insertbuiltin(st, "round", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 2, alctype(ANY_TYPE), "%s: %d, %s: %d", "n", ANY_TYPE, "r", INT_TYPE);

    entry = insertbuiltin(st, "type", FUNC_TYPE);
    add_builtin_func_info(entry, 1, 1, &string_type, "%s: %d", "x", ANY_TYPE);


    // Add string methods to string
    entry = insertbuiltin(st, "str", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &string_type, "%s: %d", "a", ANY_TYPE);
    
    // Add list methods to list
    entry = insertbuiltin(st, "list", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &list_type, "%s: %d", "a", ANY_TYPE);

    // Add file methods to file
    entry = insertbuiltin(st, "file", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &file_type, "%s: %d", "f", STRING_TYPE);

    // Add dict methods to dict
    entry = insertbuiltin(st, "dict", CLASS_TYPE);
    add_builtin_func_info(entry, 0, 1, &dict_type, "%s: %d", "a", ANY_TYPE);
}
