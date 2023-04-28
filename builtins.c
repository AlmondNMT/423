#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"
#include "type.h"

/**
 * 1. Add param count
 * 2. Add param types
 * 3. Add return type
*/
void add_builtin_func_info(SymbolTableEntry entry, int nparams, int returntype, char *fmt, ...)
{
    // Add the function return type
    if(entry->typ == NULL)
        return;
    entry->typ->u.f.returntype = alcbuiltin(returntype);
    entry->typ->u.f.nparams = nparams;
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
                            params = alcparam(name, basetype);
                            entry->typ->u.f.parameters = params;
                        }
                        else if(params != NULL && name != NULL) {
                            params->next = alcparam(name, basetype);
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
 * Add constructor information. This wraps add_builtin_func_info
*/
void add_builtin_class_info(SymbolTableEntry entry, int nparams, char *fmt, ...)
{
    if(entry == NULL || entry->typ == NULL) return;
    
    // We will not use an explicit __init__ method as is typically found in 
}

/**
 * Manually add every builtin and its associated fields
 */
void add_puny_builtins(SymbolTable st) {
    SymbolTableEntry entry = NULL;

    entry = insertbuiltin(st, "any", CLASS_TYPE);

    entry = insertbuiltin(st, "print", FUNC_TYPE);
    add_builtin_func_info(entry, 1, NONE_TYPE, "%s: %d", "s", ANY_TYPE);

    insertbuiltin(st, "None", CLASS_TYPE);
    insertbuiltin(st, "int", CLASS_TYPE);

    entry = insertbuiltin(st, "abs", FUNC_TYPE);
    add_builtin_func_info(entry, 1, ANY_TYPE, "%s: %d", "n", ANY_TYPE);

    insertbuiltin(st, "bool", CLASS_TYPE);  
    entry = insertbuiltin(st, "chr", FUNC_TYPE);
    add_builtin_func_info(entry, 1, STRING_TYPE, "%s: %d ", "n", INT_TYPE);

    insertbuiltin(st, "float", CLASS_TYPE);
    entry = insertbuiltin(st, "input", FUNC_TYPE);
    add_builtin_func_info(entry, 1, STRING_TYPE, "%s: %d", "s", STRING_TYPE);

    insertbuiltin(st, "int", CLASS_TYPE);    

    entry = insertbuiltin(st, "len", FUNC_TYPE);
    add_builtin_func_info(entry, 1, INT_TYPE, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "max", FUNC_TYPE);
    add_builtin_func_info(entry, 1, INT_TYPE, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "min", FUNC_TYPE);
    add_builtin_func_info(entry, 1, INT_TYPE, "%s: %d", "l", LIST_TYPE);

    entry = insertbuiltin(st, "open", FUNC_TYPE);
    add_builtin_func_info(entry, 2, FILE_TYPE, "%s: %d, %s: %d", "pathname", STRING_TYPE, "mode", STRING_TYPE);

    entry = insertbuiltin(st, "ord", FUNC_TYPE);
    add_builtin_func_info(entry, 1, INT_TYPE, "%s: %d", "s", STRING_TYPE);

    entry = insertbuiltin(st, "pow", FUNC_TYPE);
    add_builtin_func_info(entry, 2, ANY_TYPE, "%s: %d, %s: %d", "b", ANY_TYPE, "e", ANY_TYPE);

    insertbuiltin(st, "range", CLASS_TYPE);

    entry = insertbuiltin(st, "round", FUNC_TYPE);
    add_builtin_func_info(entry, 2, ANY_TYPE, "%s: %d, %s: %d", "n", ANY_TYPE, "r", INT_TYPE);

    insertbuiltin(st, "type", CLASS_TYPE);

    // Add string methods to string
    entry = insertbuiltin(st, "str", CLASS_TYPE);
    entry->typ = alcstr();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;
    
    // Add list methods to list
    entry = insertbuiltin(st, "list", CLASS_TYPE);
    // This adds "append" and "remove" to the list type symbol table
    entry->typ = alclist();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add file methods to file
    entry = insertbuiltin(st, "file", CLASS_TYPE);
    entry->typ = NULL;
    entry->typ = alcfile();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add dict methods to dict
    entry = insertbuiltin(st, "dict", CLASS_TYPE);
    free(entry->typ);
    entry->typ = (struct typeinfo *) alcdict();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;
}
