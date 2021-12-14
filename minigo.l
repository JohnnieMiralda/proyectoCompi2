%option noyywrap
%option caseless
%option yylineno
%option nodefault

%{
    #include <stdio.h>
    #include <string>
    #include "tokens.h"
    using namespace std;

%}

%x CommentBlock


%%
"//"[^\n]* ;
"/*" {BEGIN(CommentBlock);}
"break" {return BREAK_TK;}
"continue" {return CONT_TK;}
"else" {return ELSE_TK;}
"for" {return FOR_TK;}
"func" {return FUNC_TK;}
"if" {return IF_TK;}
"import" {return IMPOR_TK;}
"package" {return PACK_TK;}
"return" {return RETU_TK;}
"var" {return VAR_TK;}
"true" {return TRUE_TK;}
"false" {return FALSE_TK;}
"main" {return MAIN_TK;}
"int" {return INT_TK;}
"bool" {return BOOL_TK;}
"string" {return STRING_TK;}
"float" {return FLOAT_TK;}
"fmt.Println" {return PRINT_TK;}

"++" { return PLUS_PLUS_TK;}
"+=" { return PLUS_EQUAL_TK;}
"+" { return '+';}

"--" { return MINUS_MINUS_TK;}
"-=" { return MINUS_EQUAL_TK;}
"-" { return '-';}

"&=" { return AND_EQUAL_TK;}
"&&" { return AND_TK;}

"!=" { return NOT_EQUAL_TK;}
"!" { return '!';}

"<=" { return LESS_OR_EQUAL_TK;}
">=" { return GREATER_OR_EQUAL_TK;}
"<" { return '<';}
">" { return '>';}

":=" { return COLON_EQUAL_TK;}
":" { return ':';}

"/=" { return SLASH_EQUAL_TK;}
"/" { return '/';}

"||" { return OR_TK;}
"|=" { return OR_EQUAL_TK;}

"*=" { return MULT_EQUAL_TK;}
"*" { return '*';}

"^=" { return TRAN_EQUAL_TK;}
"^" { return '^';}

"%=" { return PORC_EQUAL_TK;}
"%" { return '%';}


"==" { return EQUALS_TK;}
"=" { return '=';}


"(" { return '(';}
")" { return ')';}

"{" { return '{';}
"}" { return '}';}
"[" { return '[';}
"]" { return ']';}
"." { return '.';}
"," { return ',';}
";" { return ';';}

\".*\" { yylval.string_t = new string(yytext); return LIT_STRING_TK;}
[0-9]+ {yylval.int_t = atoi(yytext); return LIT_INT_TK;}
[0-9]+"."[0-9]+ {yylval.float_t = atof(yytext); return LIT_FLOAT_TK;}
[a-zA-Z]+[a-zA-Z0-9]* {yylval.string_t = new string(yytext); return ID_TK;}


[\n]+ ;
[ ] ;
. ;
<CommentBlock>"*/" {BEGIN(INITIAL);}
<CommentBlock>.|\n {  }
<CommentBlock><<EOF>> { printf("Comentario de bloque no cerrado\n");}
%%
