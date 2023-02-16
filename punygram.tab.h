/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PUNYGRAM_TAB_H_INCLUDED
# define YY_YY_PUNYGRAM_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    FLOATLIT = 258,
    ENDMARKER = 259,
    NAME = 260,
    INTLIT = 261,
    STRINGLIT = 262,
    NEWLINE = 263,
    INDENT = 264,
    DEDENT = 265,
    LPAR = 266,
    RPAR = 267,
    LSQB = 268,
    RSQB = 269,
    COLON = 270,
    COMMA = 271,
    SEMI = 272,
    PLUS = 273,
    MINUS = 274,
    STAR = 275,
    SLASH = 276,
    VBAR = 277,
    AMPER = 278,
    LESS = 279,
    GREATER = 280,
    EQUAL = 281,
    DOT = 282,
    PERCENT = 283,
    LBRACE = 284,
    RBRACE = 285,
    EQEQUAL = 286,
    NOTEQUAL = 287,
    LESSEQUAL = 288,
    GREATEREQUAL = 289,
    TILDE = 290,
    CIRCUMFLEX = 291,
    LEFTSHIFT = 292,
    RIGHTSHIFT = 293,
    DOUBLESTAR = 294,
    PLUSEQUAL = 295,
    MINEQUAL = 296,
    STAREQUAL = 297,
    SLASHEQUAL = 298,
    PERCENTEQUAL = 299,
    AMPEREQUAL = 300,
    VBAREQUAL = 301,
    CIRCUMFLEXEQUAL = 302,
    LEFTSHIFTEQUAL = 303,
    RIGHTSHIFTEQUAL = 304,
    DOUBLESTAREQUAL = 305,
    DOUBLESLASH = 306,
    DOUBLESLASHEQUAL = 307,
    AT = 308,
    ATEQUAL = 309,
    RARROW = 310,
    ELLIPSIS = 311,
    COLONEQUAL = 312,
    PYFALSE = 313,
    PYDEF = 314,
    IF = 315,
    RAISE = 316,
    NONE = 317,
    DEL = 318,
    IMPORT = 319,
    RETURN = 320,
    PYTRUE = 321,
    ELIF = 322,
    IN = 323,
    TRY = 324,
    AND = 325,
    ELSE = 326,
    IS = 327,
    WHILE = 328,
    AS = 329,
    EXCEPT = 330,
    LAMBDA = 331,
    WITH = 332,
    ASSERT = 333,
    FINALLY = 334,
    NONLOCAL = 335,
    YIELD = 336,
    BREAK = 337,
    FOR = 338,
    NOT = 339,
    CLASS = 340,
    FROM = 341,
    OR = 342,
    CONTINUE = 343,
    GLOBAL = 344,
    PASS = 345,
    OP = 346,
    AWAIT = 347,
    ASYNC = 348,
    TYPE_IGNORE = 349,
    TYPE_COMMENT = 350,
    ERRORTOKEN = 351,
    COMMENT = 352,
    ENCODING = 353
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PUNYGRAM_TAB_H_INCLUDED  */
