%{
//http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
    #include <cstdio>
    using namespace std;
    int yylex();
    extern int yylineno;
    void yyerror(const char * s){
        fprintf(stderr, "Line: %d, error: %s \n", yylineno, s);
    }

    #define YYERROR_VERBOSE 1
    #define YYDEBUG 1
    #define EQUAL 1
    #define PLUSEQUAL 2
    #define MINUSEQUAL 3
%}


%union{
    int int_t;
    float float_t;
    const char * string_t;
    bool bool_t;
}


%token BREAK_TK
%token CONT_TK
%token ELSE_TK
%token FUNC_TK
%token IF_TK
%token IMPOR_TK
%token PACK_TK
%token RETU_TK
%token VAR_TK
%token TRUE_TK
%token FALSE_TK
%token INT_TK
%token FLOAT_TK
%token STRING_TK
%token BOOL_TK
%token FOR_TK
%token<string_t> LIT_STRING_TK
%token<int_t> LIT_INT_TK 
%token<float_t> LIT_FLOAT_TK 
%token<string_t> ID_TK
%token PLUS_PLUS_TK PLUS_EQUAL_TK
%token MINUS_MINUS_TK MINUS_EQUAL_TK
%token AND_EQUAL_TK AND_TK
%token NOT_EQUAL_TK
%token LESS_OR_EQUAL_TK GREATER_OR_EQUAL_TK
%token COLON_EQUAL_TK
%token SLASH_EQUAL_TK
%token OR_TK OR_EQUAL_TK
%token MULT_EQUAL_TK
%token TRAN_EQUAL_TK
%token PORC_EQUAL_TK
%token EQUALS_TK
%token MAIN_TK
%%

start: PACK_TK MAIN_TK import_loop body {printf("sintaxis correcta\n");}
    ;

import_loop: IMPOR_TK '(' import_loop
    | LIT_STRING_TK ')'
    | IMPOR_TK LIT_STRING_TK
    | LIT_STRING_TK import_loop
    ;

body: FUNC_TK MAIN_TK '('')'  block_statements  body
    | FUNC_TK ID_TK '(' param_list ')' type  block_statements body
    | FUNC_TK ID_TK '(' param_list ')' '[' ']' type  block_statements body
    | declaration_statement body
    |
    ;

statement_list: statement_list statement
    | statement
    ;

statement: declaration_statement
    | asignation_statement
    | BREAK_TK
    | CONT_TK
    | if_statement
    | jump_statement
    | FOR_TK for_statement
    | methodcall
    | block_statements
    ;

declaration_statement: VAR_TK ID_TK type
    | VAR_TK ID_TK type '=' expression
    | VAR_TK ID_TK '=' expression
    | ID_TK COLON_EQUAL_TK expression
    ;

asignation_statement:ID_TK assignment_operator expression
    | ID_TK '[' expression ']' assignment_operator expression
    | unary_statement 
    ;

unary_statement: ID_TK PLUS_PLUS_TK
    | ID_TK MINUS_MINUS_TK
    ;

jump_statement: RETU_TK expression
    ;

for_statement: declaration_statement ';' expression ';' asignation_statement 
    | block_statements
    | expression statement  
    ;

methodcall: ID_TK '.' ID_TK '('')'
    | ID_TK '.' ID_TK '(' expression_list ')'
    | ID_TK '(' expression_list ')'
    ;

expression_list: expression_list ',' expression
    | expression

if_statement: IF_TK expression  '{' statement_list '}' else_statement
    ;

else_statement: ELSE_TK statement
    |
    ;

block_statements: '{' statement_list '}'
    ;

type: INT_TK
    |BOOL_TK
    |STRING_TK
    |FLOAT_TK
    ;

param_list: param_list ',' ID_TK type
    | param_list ',' ID_TK '['']'type 
    | ID_TK type
    | ID_TK '['']'type
    | 
    ;

expression: expression AND_TK unary_expression
    | expression OR_TK unary_expression
    | expression EQUALS_TK unary_expression
    | expression NOT_EQUAL_TK unary_expression
    | unary_expression
    ;

unary_expression: '!' relational_expression 
    | relational_expression
    ;

relational_expression: relational_expression '<' additive_expression
    | relational_expression LESS_OR_EQUAL_TK additive_expression
    | relational_expression '>' additive_expression
    | relational_expression GREATER_OR_EQUAL_TK additive_expression
    | relational_expression '^'additive_expression
    | additive_expression
    ;

additive_expression: additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    | multiplicative_expression
    ;

multiplicative_expression: multiplicative_expression '*' primary_expression
    | multiplicative_expression '/' primary_expression
    | multiplicative_expression '%' primary_expression
    | primary_expression
    ;

primary_expression: '('expression ')'
    | ID_TK 
    | ID_TK '[' expression ']'
    | LIT_STRING_TK
    | LIT_INT_TK
    | LIT_FLOAT_TK
    | TRUE_TK
    | FALSE_TK
    | '['']' type '{' expression_list '}' ';'
    | methodcall
    ;


assignment_operator: '=' 
    | PLUS_EQUAL_TK
    | MINUS_EQUAL_TK 
    | MULT_EQUAL_TK 
    | SLASH_EQUAL_TK 
    | PORC_EQUAL_TK 
    | TRAN_EQUAL_TK 
    ;








