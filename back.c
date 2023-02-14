#include "punygram.tab.h"

//returns name of category for category integer. may prove handy in the future.

char *rev_token(int cat){

    switch(cat)
    {
        case   FLOATLIT: return "FLOATLIT";          
        case   ENDMARKER: return "ENDMARKER";         
        case   NAME: return "NAME";              
        case   INTLIT: return "INTLIT";            
        case   STRINGLIT: return "STRINGLIT";         
        case   NEWLINE: return "NEWLINE";           
        case   INDENT: return "INDENT";            
        case   DEDENT: return "DEDENT";            
        case   LPAR: return "LPAR";              
        case   RPAR: return "RPAR";              
        case   LSQB: return "LSQB";              
        case   RSQB : return "RSQB";             
        case   COLON : return "COLON";            
        case   COMMA: return "COMMA";             
        case   SEMI: return "SEMI";              
        case   PLUS: return "PLUS";              
        case   MINUS: return "MINUS";             
        case   STAR: return "STAR";              
        case   SLASH: return "SLASH";             
        case   VBAR: return "VBAR";              
        case   AMPER: return "AMPER";             
        case   LESS: return "LESS";              
        case   GREATER: return "GREATER";           
        case   EQUAL: return "EQUAL";             
        case   DOT: return "DOT";               
        case   PERCENT: return "PERCENT";           
        case   LBRACE: return "LBRACE";            
        case   RBRACE: return "RBRACE";            
        case   EQEQUAL: return "EQEQUAL";           
        case   NOTEQUAL: return "NOTEQUAL";          
        case   LESSEQUAL: return "LESSEQUAL";         
        case   GREATEREQUAL: return "GREATEREQUAL";      
        case   TILDE: return "TILDE";             
        case   CIRCUMFLEX: return "CIRCUMFLEX";        
        case   LEFTSHIFT: return "LEFTSHIFT";         
        case   RIGHTSHIFT: return "RIGHTSHIFT";        
        case   DOUBLESTAR: return "DOUBLESTAR";        
        case   PLUSEQUAL: return "PLUSEQUAL";         
        case   MINEQUAL: return "MINEQUAL";          
        case   STAREQUAL: return "STAREQUAL";         
        case   SLASHEQUAL: return "SLASHEQUAL";        
        case   PERCENTEQUAL: return "PERCENTEQUAL";      
        case   AMPEREQUAL: return "AMPEREQUAL";        
        case   VBAREQUAL: return "VBAREQUAL";         
        case   CIRCUMFLEXEQUAL: return "CIRCUMFLEXEQUAL";   
        case   LEFTSHIFTEQUAL: return "LEFTSHIFTEQUAL";    
        case   RIGHTSHIFTEQUAL: return "RIGHTSHIFTEQUAL";   
        case   DOUBLESTAREQUAL: return "DOUBLESTAREQUAL";   
        case   DOUBLESLASH: return "DOUBLESLASH";       
        case   DOUBLESLASHEQUAL: return "DOUBLESLASHEQUAL";  
        case   AT: return "AT";                
        case   ATEQUAL: return "ATEQUAL";           
        case   RARROW: return "RARROW";            
        case   ELLIPSIS: return "ELLIPSIS";          
        case   COLONEQUAL: return "COLONEQUAL";        

        case   PYFALSE: return "PYFALSE";           
        case   PYDEF: return "PYDEF";	          
        case   IF: return "IF";	              
        case   RAISE: return "RAISE";             
        case   NONE: return "NONE";	          
        case   DEL: return "DEL";	              
        case   IMPORT: return "IMPORT";	          
        case   RETURN: return "RETURN";            
        case   PYTRUE: return "PYTRUE";	          
        case   ELIF: return "ELIF";	          
        case   IN: return "IN";	              
        case   TRY: return "TRY";               
        case   AND: return "AND";	              
        case   ELSE: return "ELSE";	          
        case   IS: return "IS";	              
        case   WHILE: return "WHILE";             
        case   AS: return "AS";	              
        case   EXCEPT: return "EXCEPT";	          
        case   LAMBDA: return "LAMBDA";	          
        case   WITH: return "WITH";              
        case   ASSERT: return "ASSERT";	          
        case   FINALLY: return "FINALLY";	          
        case   NONLOCAL: return "NONLOCAL";	      
        case   YIELD: return "YIELD";             
        case   BREAK: return "BREAK";	          
        case   FOR: return "FOR";	              
        case   NOT: return "NOT";	              
        case   CLASS: return "CLASS";	          
        case   FROM: return "FROM";	          
        case   OR: return "OR";	              
        case   CONTINUE: return "CONTINUE";	      
        case   GLOBAL: return "GLOBAL";	          
        case   PASS: return "PASS";              
        case   OP: return "OP";                
        case   AWAIT: return "AWAIT";             
        case   ASYNC: return "ASYNC";             
        case   TYPE_IGNORE: return "TYPE_IGNORE";       
        case   TYPE_COMMENT: return "TYPE_COMMENT";      
        case   ERRORTOKEN: return "ERRORTOKEN";        

        case   COMMENT: return "COMMENT";           
        case   NL: return "NL";                
        case   ENCODING: return "ENCODING"; 
    }
    return "ERROR UNKNOWN CASE IN back.c, rev_token()";  
}       
