#define FLOATLIT            259
#define ENDMARKER           260
#define NAME                261
#define INTLIT              262
#define STRINGLIT           263
#define NEWLINE             264
#define INDENT              265
#define DEDENT              500 
 
#define LPAR                266       //'('
#define RPAR                267       //')'
#define LSQB                268       //'['
#define RSQB                269       //']'
#define COLON               270       //':'
#define COMMA               271       //','
#define SEMI                272       //';'
#define PLUS                273       //'+'
#define MINUS               274       //'-'
#define STAR                275       //'*'
#define SLASH               276       //'/'
#define VBAR                277       //'|'
#define AMPER               278       //'&'
#define LESS                279       //'<'
#define GREATER             280       //'>'
#define EQUAL               281       //'='
#define DOT                 282       //'.'
#define PERCENT             283       //'%'
#define LBRACE              284       //'{'
#define RBRACE              285       //'}'
#define EQEQUAL             286       //'=='
#define NOTEQUAL            287       //'!='
#define LESSEQUAL           288       //'<='
#define GREATEREQUAL        289       //'>='
#define TILDE               290       //'~'
#define CIRCUMFLEX          291       //'^'
#define LEFTSHIFT           292       //'<<'
#define RIGHTSHIFT          293       //'>>'
#define DOUBLESTAR          294       //'**'
#define PLUSEQUAL           295       //'+='
#define MINEQUAL            296       //'-='
#define STAREQUAL           297       //'*='
#define SLASHEQUAL          298       //'/='
#define PERCENTEQUAL        299       //'%='
#define AMPEREQUAL          300       //'&='
#define VBAREQUAL           301       //'|='
#define CIRCUMFLEXEQUAL     302       //'^='
#define LEFTSHIFTEQUAL      303       //'<<='
#define RIGHTSHIFTEQUAL     304       //'>>='
#define DOUBLESTAREQUAL     305       //'**='
#define DOUBLESLASH         306       //'//'
#define DOUBLESLASHEQUAL    307       //'//='
#define AT                  308       //'@'
#define ATEQUAL             309       //'@='
#define RARROW              310       //'->'
#define ELLIPSIS            311       //'...'
#define COLONEQUAL          312       //':='

//NOTE: None of the following had strikethroughs in the 
//punyref.html you gave us. Those that will have strikethroughs
//in the future will lead to an error being thrown ("command not part of puny")

#define PYFALSE             400
#define PYDEF	            401
#define IF	                402
#define RAISE               403
#define NONE	            404
#define DEL	                405
#define IMPORT	            406
#define RETURN              407
#define PYTRUE	            408
#define ELIF	            409
#define IN	                410
#define TRY                 411
#define AND	                412
#define ELSE	            413
#define IS	                414
#define WHILE               415
#define AS	                416
#define EXCEPT	            417
#define LAMBDA	            418
#define WITH                419
#define ASSERT	            420
#define FINALLY	            421
#define NONLOCAL	        422
#define YIELD               423
#define BREAK	            424
#define FOR	                425
#define NOT	                426
#define CLASS	            427
#define FROM	            428
#define OR	                429
#define CONTINUE	        430
#define GLOBAL	            431
#define PASS                432

#define OP                 313 
#define AWAIT              314 
#define ASYNC              315 
#define TYPE_IGNORE        316 
#define TYPE_COMMENT       317       
#define ERRORTOKEN         318 
 
//These aren't used by the C tokenizer but are needed for tokenize.py
//I dont know what the above comment means, but there are now return values for these

#define COMMENT             319
#define NL                  320
#define ENCODING            321