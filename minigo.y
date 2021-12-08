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
    | asignation_statement body
    |
    ;

statement_list: statement_list statement
    | statement
    ;

statement: asignation_statement
    | if_statement
    | block_statements
    | jump_statement
    | FOR_TK for_statement
    | methodcall
    | expression
    | BREAK_TK
    | CONT_TK
    ;

methodcall: ID_TK '.' ID_TK '('')'
    | ID_TK '.' ID_TK '(' expression_list ')'
    ;

expression_list: expression_list ',' unary_expression
    | unary_expression

for_statement:  expression statement
    | statement
    | assignment_expression ';' expression ';' additive_expression statement

asignation_statement: VAR_TK assignment_expression
    | assignment_expression
    ;

if_statement: IF_TK expression  '{' statement_list '}' else_statement
    ;

else_statement: ELSE_TK statement
    |
    ;

block_statements: '{' statement_list '}'
    ;

jump_statement: RETU_TK expression
    ;

type: INT_TK
    |BOOL_TK
    |STRING_TK
    |FLOAT_TK
    |
    ;

param_list: param_list ',' ID_TK type
    | param_list ',' ID_TK '['']'type 
    | ID_TK type
    | ID_TK '['']'type
    | 
    ;

assignment_expression: unary_expression type assignment_operator assignment_expression
    | logical_or_expression 
    ;

logical_or_expression: logical_or_expression OR_TK logical_and_expression 
    | logical_and_expression
    ;

logical_and_expression: logical_and_expression AND_TK equality_expression 
    | equality_expression 
    ;

equality_expression:  equality_expression EQUALS_TK relational_expression 
    | equality_expression NOT_EQUAL_TK relational_expression 
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

multiplicative_expression: multiplicative_expression '*' unary_expression
    | multiplicative_expression '/' unary_expression
    | multiplicative_expression '%' unary_expression
    | unary_expression
    ;

unary_expression: '!' unary_expression  
    | postfix_expression
    ;


postfix_expression: primary_expression 
    | primary_expression '.' primary_expression '(' argument_expression_list ')'
    | postfix_expression '[' expression ']' 
    | postfix_expression '(' ')' 
    | postfix_expression '(' argument_expression_list ')' 
    | postfix_expression PLUS_PLUS_TK
    | postfix_expression MINUS_EQUAL_TK
    ;

primary_expression: '('expression ')'
    | ID_TK 
    | LIT_STRING_TK
    | LIT_INT_TK
    | LIT_FLOAT_TK
    | TRUE_TK
    | FALSE_TK
    | '['']' type '{' expression_list '}' ';'
    ;

argument_expression_list: argument_expression_list ',' assignment_expression 
    | assignment_expression
    | 
    ;

assignment_operator: '=' 
    | PLUS_EQUAL_TK
    | MINUS_EQUAL_TK 
    | MULT_EQUAL_TK 
    | SLASH_EQUAL_TK 
    | PORC_EQUAL_TK 
    | TRAN_EQUAL_TK 
    | COLON_EQUAL_TK
    ;

expression: assignment_expression
    ;






