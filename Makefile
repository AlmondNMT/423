CC=gcc
CFLAGS=-g -Wall -c

all: bison flex compile link

bison: punygram.y
	bison punygram.y -Wconflicts-rr -Wcounterexamples
	bison -d punygram.y

flex: punylex.l punygram.tab.h tree.h utils.h
	flex punylex.l

compile: lex.yy.c punygram.tab.c utils.c main.c back.c tree.c utils.h
	$(CC) $(CFLAGS) punygram.tab.c
	$(CC) $(CFLAGS) lex.yy.c 
	$(CC) $(CFLAGS) utils.c
	$(CC) $(CFLAGS) main.c
	$(CC) $(CFLAGS) back.c
	$(CC) $(CFLAGS) tree.c

link: lex.yy.o utils.o main.o back.o tree.o punygram.tab.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o tree.o main.o back.o -o puny
	./puny test.py

tests: tests.c
	gcc -Wall -g tests.c -o tests

clean: 
	rm -f lex.yy.c puny tests *.o a.out punygram.tab.*

zip: punylex.l main.c utils.c back.c tree.c defs.h tree.h utils.h Makefile
	zip hw2.zip punylex.l main.c utils.c back.c tree.c defs.h tree.h utils.h Makefile
