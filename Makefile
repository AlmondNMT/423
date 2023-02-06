all: flex compile link

flex: punylex.l token.h defs.h utils.h
	flex punylex.l

compile: lex.yy.c utils.c main.c back.c token.c defs.h utils.h
	gcc -g -Wall -c lex.yy.c 
	gcc -g -Wall -c utils.c
	gcc -g -Wall -c main.c
	gcc -g -Wall -c back.c
	gcc -g -Wall -c token.c

link: lex.yy.o utils.o main.o back.o token.o
	gcc -g -Wall lex.yy.o utils.o token.o main.o back.o -o puny

tests: tests.c
	gcc -Wall -g tests.c -o tests

clean:
	rm main.o lex.yy.c lex.yy.o puny tests

zip: punylex.l main.c utils.c back.c token.c defs.h token.h utils.h Makefile
	zip hw2.zip punylex.l main.c utils.c back.c token.c defs.h token.h utils.h Makefile
