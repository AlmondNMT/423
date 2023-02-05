all: punylex.l main.c
	flex punylex.l
	gcc -Wall -c lex.yy.c 
	gcc -Wall -c main.c
	gcc lex.yy.o main.o -o puny

tests: tests.c
	gcc -Wall -g tests.c -o tests

clean:
	rm main.o lex.yy.c lex.yy.o puny tests
