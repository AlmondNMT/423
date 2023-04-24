#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"
#include "type.h"

/**
 * Manually add every builtin and its associated fields
 */
void add_puny_builtins(SymbolTable st) {
    SymbolTableEntry entry = NULL;

    insertbuiltin(st, "any", -1, "(builtins)", CLASS_TYPE);

    entry = insertbuiltin(st, "print", -1, "(builtins)", FUNC_TYPE);
    entry->typ->u.f.returntype = alcnone();

    insertbuiltin(st, "None", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "int", -1, "(builtins)", CLASS_TYPE);
    entry = insertbuiltin(st, "abs", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "bool", -1, "(builtins)", CLASS_TYPE);  
    insertbuiltin(st, "chr", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "float", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "input", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "int", -1, "(builtins)", CLASS_TYPE);    
    insertbuiltin(st, "len", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "max", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "min", -1, "(builtins)", FUNC_TYPE);
    entry = insertbuiltin(st, "open", -1, "(builtins)", FUNC_TYPE);
    entry->typ->u.f.returntype = alcfile();
    insertbuiltin(st, "ord", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "pow", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "range", -1, "(builtins)", CLASS_TYPE);
    insertbuiltin(st, "round", -1, "(builtins)", FUNC_TYPE);
    insertbuiltin(st, "type", -1, "(builtins)", CLASS_TYPE);

    // Add string methods to string
    entry = insertbuiltin(st, "str", -1, "(builtins)", CLASS_TYPE);
    entry->typ = alcstr();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;
    
    // Add list methods to list
    entry = insertbuiltin(st, "list", -1, "(builtins)", CLASS_TYPE);
    // This adds "append" and "remove" to the list type symbol table
    entry->typ = alclist();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add file methods to file
    entry = insertbuiltin(st, "file", -1, "(builtins)", CLASS_TYPE);
    entry->typ = NULL;
    entry->typ = alcfile();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;

    // Add dict methods to dict
    entry = insertbuiltin(st, "dict", -1, "(builtins)", CLASS_TYPE);
    free(entry->typ);
    entry->typ = (struct typeinfo *) alcdict();
    entry->typ->basetype = CLASS_TYPE;
    entry->nested = entry->typ->u.cls.st;
    entry->nested->parent = st;
    entry->nested->level = st->level + 1;
}
