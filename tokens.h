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

#ifndef YY_YY_TOKENS_H_INCLUDED
# define YY_YY_TOKENS_H_INCLUDED
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
    BREAK_TK = 258,
    CONT_TK = 259,
    ELSE_TK = 260,
    FUNC_TK = 261,
    IF_TK = 262,
    IMPOR_TK = 263,
    PACK_TK = 264,
    RETU_TK = 265,
    VAR_TK = 266,
    TRUE_TK = 267,
    FALSE_TK = 268,
    INT_TK = 269,
    FLOAT_TK = 270,
    STRING_TK = 271,
    BOOL_TK = 272,
    FOR_TK = 273,
    LIT_STRING_TK = 274,
    LIT_INT_TK = 275,
    LIT_FLOAT_TK = 276,
    ID_TK = 277,
    PLUS_PLUS_TK = 278,
    PLUS_EQUAL_TK = 279,
    MINUS_MINUS_TK = 280,
    MINUS_EQUAL_TK = 281,
    AND_EQUAL_TK = 282,
    AND_TK = 283,
    NOT_EQUAL_TK = 284,
    LESS_OR_EQUAL_TK = 285,
    GREATER_OR_EQUAL_TK = 286,
    COLON_EQUAL_TK = 287,
    SLASH_EQUAL_TK = 288,
    OR_TK = 289,
    OR_EQUAL_TK = 290,
    MULT_EQUAL_TK = 291,
    TRAN_EQUAL_TK = 292,
    PORC_EQUAL_TK = 293,
    EQUALS_TK = 294,
    MAIN_TK = 295
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "minigo.y"

    int int_t;
    float float_t;
    const char * string_t;
    bool bool_t;

#line 105 "tokens.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOKENS_H_INCLUDED  */
