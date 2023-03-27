CC=gcc
CFLAGS=-g -Wall -c
CFILES=punygram.tab.c lex.yy.c utils.c main.c back.c tree.c symtab.c printsyms.c type.c

all: bison flex compile link 

bison: punygram.y
	bison --debug punygram.y -v
	bison -d --debug punygram.y

flex: punylex.l punygram.tab.h tree.h utils.h
	flex punylex.l

compile: lex.yy.c punygram.tab.c utils.c main.c back.c tree.c symtab.c utils.h symtab.h symtab.c printsyms.c type.c type.h
	$(CC) $(CFLAGS) $(CFILES)

link: lex.yy.o utils.o main.o back.o tree.o punygram.tab.o symtab.o type.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o tree.o main.o back.o type.o symtab.o -o puny
	./puny test.py

tests: tests.c punylex.l punygram.y utils.c back.c tree.c symtab.c tree.h utils.h symtab.h
	bison punygram.y
	bison -d punygram.y
	flex punylex.l
	$(CC) $(CFLAGS) $(CFILES)
	gcc -Wall -g lex.yy.o utils.o back.o tree.o punygram.tab.o symtab.o tests.c -o tests
	./tests

testsnolexing:
	gcc -Wall -g  utils.c back.c tree.c tests.c 
	gcc -Wall -g  utils.o back.o tree.o tests.o type.o -o tests

clean: 
	rm -f lex.yy.c puny tests *.o a.out punygram.tab.*

zip: punylex.l punygram.y main.c utils.c back.c tree.c tree.h utils.h Makefile printsyms.c symtab.h symtab.c test.py
	zip lab6.zip punygram.y punylex.l main.c utils.c back.c tree.c tree.h utils.h Makefile symtab.h symtab.c printsyms.c errdef.h type.c type.h test.py fdsa.py
	# Don't forget to remove test.py and fdsa.py for the HWs
