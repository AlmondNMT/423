CC=gcc
CFLAGS=-g -Wall -c
CFILES=punygram.tab.c lex.yy.c utils.c main.c back.c tree.c

all: bison flex compile link 

bison: punygram.y
	bison punygram.y -v
	bison -d punygram.y

flex: punylex.l punygram.tab.h tree.h utils.h
	flex punylex.l

compile: lex.yy.c punygram.tab.c utils.c main.c back.c tree.c utils.h
	$(CC) $(CFLAGS) $(CFILES)

link: lex.yy.o utils.o main.o back.o tree.o punygram.tab.o
	gcc -g -Wall lex.yy.o punygram.tab.o utils.o tree.o main.o back.o -o puny
	#./puny test.py

tests: tests.c
	$(bison)
	$(flex)
	$(compile)
	gcc -Wall -g lex.yy.o utils.o back.o tree.o punygram.tab.o tests.c -o tests
	./tests


testsnolexing:
	gcc -Wall -g  utils.c back.c tree.c tests.c 
	gcc -Wall -g  utils.o back.o tree.o tests.o -o tests



clean: 
	rm -f lex.yy.c puny tests *.o a.out punygram.tab.*

zip: punylex.l punygram.y main.c utils.c back.c tree.c tree.h utils.h Makefile
	zip hw3.zip punygram.y punylex.l main.c utils.c back.c tree.c tree.h utils.h Makefile
