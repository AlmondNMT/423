CC=gcc
CFLAGS=-g -Wall -c

all: bison flex compile link

bison: punygram.y
	bison punygram.y
	bison -d punygram.y

flex: punylex.l punygram.tab.h token.h utils.h
	flex punylex.l

compile: lex.yy.c punygram.tab.c utils.c main.c back.c token.c utils.h
	$(CC) $(CFLAGS) punygram.tab.c
	$(CC) $(CFLAGS) lex.yy.c 
	$(CC) $(CFLAGS) utils.c
	$(CC) $(CFLAGS) main.c
	$(CC) $(CFLAGS) back.c
	$(CC) $(CFLAGS) token.c

link: lex.yy.o utils.o main.o back.o token.o punygram.tab.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o token.o main.o back.o -o puny

tests: tests.c
	gcc -Wall -g tests.c -o tests

clean: 
	rm -f lex.yy.c puny tests *.o 

zip: punylex.l main.c utils.c back.c token.c defs.h token.h utils.h Makefile
	zip hw2.zip punylex.l main.c utils.c back.c token.c defs.h token.h utils.h Makefile
