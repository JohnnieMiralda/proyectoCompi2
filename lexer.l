%option noyywrap
%option caseless
%option yylineno
%option nodefault

%{
    #include <iostream>
    using namespace std;

%}

%x CommentBlock


%%
"//"[^\n]* ;
"/*" {BEGIN(CommentBlock);}
"break" {return TK_BREAK;}
"continue" {return TK_CONTINUE;}
"else" {return TK_ELSE ;}
"for" {return TK_FOR;}
"func" {return TK_FUNC;}
"if" {return TK_IF;}
"import" {return TK_IMPORT ;}
"package" {return TK_PAKAGE ;}
"return" {return TK_RETURN ;}
"select" {return TK_ ;}
"struct" {return TK_ ;}
"switch" {return TK_ ;}
"type" {return TK_ ;}
"var" {return TK_ ;}
"true" {printf("true keyword - lexeme %s \n",yytext);}
"false" {printf("false keyword - lexeme %s \n",yytext);}
\"[a-zA-Z0-9 ]+\" {printf("token interpreted string - lexema %s \n",yytext);}
[a-zA-Z]+[a-zA-Z0-9]* {printf("token identifier - lexema %s \n",yytext);}
['][a-zA-Z0-9]+['] {printf("token raw string - lexema %s \n",yytext);}
[0-9]+ {printf("token integer literal - lexema %s \n",yytext);}
"0"[b][_]?[0-1]+ {printf("token binary literal - lexema %s \n",yytext);} 
"0"[o][_]?[0-7]+ {printf("token octadecimal literal - lexema %s \n",yytext);}
"0"[x][_]?[0-9A-F]+ {printf("token hexadecimal literal - lexema %s \n",yytext);}
[0-9]+"."[0-9]*[e]?[+|-]?[0-9]+ |  [0-9]+[e][+|-]?[0-9]+  | "."[0-9]+[e][+|-]?[0-9]*  {printf("token decimal float literal - lexema %s \n",yytext);}
"0"[x][_]?[0-9A-F]([.][0-9A-F]+)?[p][+|-]?[0-9] | "."[0-9A-F][p][+|-]?[0-9] {printf("token hexadecimal float literal - lexema %s \n",yytext);}
[0-9]+"i" | "0"[b][_]?[0-1]+"i" | "0"[o][_]?[0-7]+"i" | "0"[x][_]?[0-9A-F]+"i" |([0-9]+"."[0-9]*[e]?[+|-]?[0-9]+ |  [0-9]+[e][+|-]?[0-9]+  | "."[0-9]+[e][+|-]?[0-9]* )+"i" | ("0"[x][_]?[0-9A-F]([.][0-9A-F]+)?[p][+|-]?[0-9] | "."[0-9A-F][p][+|-]?[0-9])+"i" {printf("token imaginary literal - lexema %s \n",yytext);}
"+" { printf("token operato o punctuation - lexema %s \n",yytext);}
"-" { printf("token operato o punctuation - lexema %s \n",yytext);}
"*" { printf("token operato o punctuation - lexema %s \n",yytext);}
"/" { printf("token operato o punctuation - lexema %s \n",yytext);}
"%" { printf("token operato o punctuation - lexema %s \n",yytext);}
"^" { printf("token operato o punctuation - lexema %s \n",yytext);}
"+=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"-=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"*=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"/=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"%=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"&=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"|=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"^=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"&&" { printf("token operato o punctuation - lexema %s \n",yytext);}
"||" { printf("token operato o punctuation - lexema %s \n",yytext);}
"++" { printf("token operato o punctuation - lexema %s \n",yytext);}
"--" { printf("token operato o punctuation - lexema %s \n",yytext);}
"==" { printf("token operato o punctuation - lexema %s \n",yytext);}
"<=" { printf("token operato o punctuation - lexema %s \n",yytext);}
">=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"<" { printf("token operato o punctuation - lexema %s \n",yytext);}
">" { printf("token operato o punctuation - lexema %s \n",yytext);}
"=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"!=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"!" { printf("token operato o punctuation - lexema %s \n",yytext);}
":=" { printf("token operato o punctuation - lexema %s \n",yytext);}
"(" { printf("token operato o punctuation - lexema %s \n",yytext);}
")" { printf("token operato o punctuation - lexema %s \n",yytext);}
"{" { printf("token operato o punctuation - lexema %s \n",yytext);}
"}" { printf("token operato o punctuation - lexema %s \n",yytext);}
"[" { printf("token operato o punctuation - lexema %s \n",yytext);}
"]" { printf("token operato o punctuation - lexema %s \n",yytext);}
"," { printf("token operato o punctuation - lexema %s \n",yytext);}
":" { printf("token operato o punctuation - lexema %s \n",yytext);}
";" { printf("token operato o punctuation - lexema %s \n",yytext);}
[\n ]+ ;
. ;
<CommentBlock>"*/" {BEGIN(INITIAL);}
<CommentBlock>.|\n {  }
<CommentBlock><<EOF>> { printf("Comentario de bloque no cerrado\n"); }
%%

int main(){
    yylex();
    return 0;
}