#include <stdio.h>

extern int rows, words, chars;
extern FILE * yyin;
extern int yylex();
extern char *yytext;

int main(int argc, char *argv[]) {
	
	yyin = fopen(argv[1], "r");
	int ret = 0;
	printf("\nTokenID\t\tValue\n---------------------------\n");
	while((ret = yylex())){
		printf("%d\t\t%s\n", ret, yytext);
	}
	yylex();
	printf("\n# rows: %d chars: %d words: %d\n", rows, chars, words);
	return 0;
}
