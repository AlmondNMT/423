CC=gcc
CFLAGS=-g -Wall -c
CFILES=punygram.tab.c lex.yy.c utils.c main.c back.c tree.c symtab.c printsyms.c type.c builtins.c nonterminal.c codegen.c

all: runtime bison flex compile link

bison: punygram.y
	bison --debug punygram.y -v
	bison -d --debug punygram.y

flex: punylex.l punygram.tab.h tree.h utils.h
	flex punylex.l

compile: runtime/runtime.icn lex.yy.c punygram.tab.c utils.c main.c back.c tree.c symtab.c utils.h symtab.h symtab.c printsyms.c type.c type.h pylib.h builtins.c nonterminal.c codegen.h nonterminal.h builtins.h codegen.h
	$(CC) $(CFLAGS) $(CFILES)

link: lex.yy.o utils.o main.o back.o tree.o punygram.tab.o symtab.o type.o builtins.o nonterminal.o codegen.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o tree.o main.o back.o type.o symtab.o builtins.o nonterminal.o codegen.o -o puny
	./puny test.py

# Compile (without linking) the Unicon code for the runtime library
runtime: 
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

zip: punylex.l punygram.y main.c utils.c back.c tree.c tree.h utils.h Makefile printsyms.c symtab.h symtab.c pylib.h nonterminal.h testrunner.sh builtins.c nonterminal.c tests/
	zip -r hw5.zip punygram.y punylex.l main.c utils.c back.c tree.c tree.h utils.h builtins.h Makefile symtab.h symtab.c printsyms.c errdef.h type.c type.h pylib.h builtins.c nonterminal.h testrunner.sh nonterminal.c tests/ *.*/ runtime/ arcade.py hangman.py minesweeper.py codegen.h codegen.c
