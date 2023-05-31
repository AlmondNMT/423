CC=gcc
CFLAGS=-g -Wall -c
CFILES=punygram.tab.c lex.yy.c utils.c main.c back.c tree.c symtab.c printsyms.c type.c builtins.c nonterminal.c

all: runtime bison flex compile link

bison: punygram.y
	bison --debug punygram.y -v
	bison -d --debug punygram.y

flex: punylex.l punygram.tab.h tree.h utils.h
	flex punylex.l

compile: lex.yy.c punygram.tab.c utils.c main.c back.c tree.c symtab.c utils.h symtab.h symtab.c printsyms.c type.c type.h builtin_list.h builtins.c nonterminal.c nonterminal.h
	$(CC) $(CFLAGS) $(CFILES)

link: lex.yy.o utils.o main.o back.o tree.o punygram.tab.o symtab.o type.o builtins.o nonterminal.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o tree.o main.o back.o type.o symtab.o builtins.o nonterminal.o -o puny
	./puny test.py

# Compile (without linking) the Unicon code for the runtime library
runtime: runtime/runtime.icn
	echo "why the fuck isn't this running???"
	unicon -c -s runtime/runtime.icn

tests: all
	./testrunner.sh
	gcc -Wall -g lex.yy.o utils.o back.o tree.o punygram.tab.o symtab.o tests.c type.o -o pytest
	./pytests

testsnolexing:
	gcc -Wall -g  utils.c back.c tree.c tests.c 
	gcc -Wall -g  utils.o back.o tree.o tests.o type.o -o tests

clean: 
	rm -f lex.yy.c puny pytests *.o a.out punygram.tab.* *.zip

zip: punylex.l punygram.y main.c utils.c back.c tree.c tree.h utils.h Makefile printsyms.c symtab.h symtab.c builtin_list.h nonterminal.h testrunner.sh builtins.c nonterminal.c tests/
	zip -r lab8.zip punygram.y punylex.l main.c utils.c back.c tree.c tree.h utils.h Makefile symtab.h symtab.c printsyms.c errdef.h type.c type.h builtin_list.h builtins.c nonterminal.h testrunner.sh nonterminal.c tests/
