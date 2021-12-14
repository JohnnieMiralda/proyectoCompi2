%code requires{
    #include "ast.h"
}

%{
//http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf
    #include <cstdio>
    #include "ast.h"
    #include <string>
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
    #define MULTEQUAL 4
    #define TRANEQUAL 5
    #define PORCEQUAL 6
    #define SLASHEQUAL 7
%}


%union{
    int int_t;
    float float_t;
    string *string_t;
    bool bool_t;

    Expr * expr_t;
    ExpressionList * expression_list_t;

    Statement * statement_t;
    StatementList * statement_list_t;

    Parameter * parameter_t;
    ParameterList * parameter_list_t;

    Declarator * declarator_t;
    Initializer * initializer_t;
    InitializerElementList * initializer_list_t;

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
%token<string_t> ID_TK MAIN_TK

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
%token PRINT_TK

%type<statement_list_t> statement_list input start body
%type<statement_t> block_statements  statement external_declaration
%type<declarator_t>declaration_statement
%type<parameter_list_t> param_list
%type<parameter_t> param
%type<int_t> type assignment_operator
%type <statement_t> if_statement for_statement expression_statement jump_statement  method_declaration asignation_statement unary_statement
%type <expression_list_t> expression_list
%type<expr_t>  methodcall primary_expression multiplicative_expression additive_expression relational_expression unary_expression expression
%%

input:start {
    list<Statement *>::iterator it = $1->begin();
    while(it != $1->end()){
        printf("semantic result: %d \n",(*it)->evaluateSemantic());
        it++;
    }
}

start: PACK_TK MAIN_TK import_loop body { $$= $4;}
    ;

import_loop: IMPOR_TK '(' import_loop
    | LIT_STRING_TK ')'
    | IMPOR_TK LIT_STRING_TK
    | LIT_STRING_TK import_loop
    ;

body: body external_declaration {$$ = $1; $$->push_back($2);}
    | external_declaration {$$ = new StatementList; $$->push_back($1);}
    ;

external_declaration: method_declaration { $$ = $1; } //ya 
    ;

method_declaration: FUNC_TK ID_TK '(' param_list ')' type  block_statements {
                    $$ = new MethodDefinition((Type)$6, *$2, *$4, $7, yylineno );
                    delete $4;
                    }
    | FUNC_TK ID_TK '('')' type  block_statements {
                     ParameterList * pm = new ParameterList;
                     $$ = new MethodDefinition((Type)$5, *$2, *pm, $6, yylineno );
                    delete pm;
                    }
    | FUNC_TK ID_TK '(' param_list ')' '[' ']' type  block_statements {
                    $$ = new MethodDefinition((Type)$8, *$2, *$4, $9, yylineno );
                    delete $4;
                    }
    | FUNC_TK ID_TK '(' ')' '[' ']' type  block_statements{
                     ParameterList * pm = new ParameterList;
                     $$ = new MethodDefinition((Type)$7, *$2, *pm, $8, yylineno );
                    delete pm;
                    }
    | FUNC_TK MAIN_TK '('')'  block_statements {
                     ParameterList * pm = new ParameterList;
                     $$ = new MethodDefinition((Type)VOID, *$2, *pm, $5, yylineno );
                    delete pm;
                    }
    ;

statement_list: statement_list statement {$$ = $1; $$->push_back($2);}
    | statement {  $$ = new StatementList; $$->push_back($1); }
    ;

statement: declaration_statement {$$ = $1;} //hacerlo desde 0
    | expression_statement {$$ = $1;}  //ya 
    | asignation_statement { $$ = $1;}
    | BREAK_TK { $$= new BreakStatement(yylineno); }
    | CONT_TK { $$= new ContinueStatement(yylineno); }
    | if_statement {$$ = $1;} // ya 
    | jump_statement {$$ = $1;} // ya
    | FOR_TK for_statement {$$ = $2;} // cambiarlo pq este era while en el del inge
    | block_statements {$$ = $1;} //ya
    ;

declaration_statement: VAR_TK ID_TK type { $$ = new Declarator((Type)$3, *$2, NULL, false, yylineno);}
    | VAR_TK ID_TK type '=' expression { $$ = new Declarator( (Type)$3, *$2, new Initializer($5,yylineno), false, yylineno);}
    | VAR_TK ID_TK '=' expression { $$ = new Declarator( $4->getType(), *$2, new Initializer($4,yylineno), false, yylineno);}
    // | VAR_TK ID_TK '['']' '=' expression 
    | ID_TK COLON_EQUAL_TK expression {$$ = new Declarator( $3->getType(),*$1 , new Initializer($3,yylineno), false, yylineno);}
    ;

expression_statement: methodcall { $$ = new ExprStatement($1, yylineno);}


asignation_statement: ID_TK assignment_operator expression{
                        if($2 == EQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 1, $3);
                        }else if($2 == PLUSEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 2, $3);
                        }else if($2 == MINUSEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 3, $3);
                        }else if($2 == MULTEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 4, $3);
                        }else if($2 == TRANEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 5, $3);
                        }else if($2 == PORCEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 6, $3);
                        }else if($2 == SLASHEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 7, $3);
                        }else if($2 == SLASHEQUAL){
                            $$ = new AsignationStatement(new IdExpr(*$1, yylineno), 7, $3);
                        }
                    }
    //| ID_TK '[' expression ']' assignment_operator expression
    | unary_statement { $$ = $1;}
    ;

unary_statement: ID_TK PLUS_PLUS_TK { $$ = new PostIncrementExpr(new IdExpr(*$1,yylineno), yylineno); }
    | ID_TK MINUS_MINUS_TK { $$ = new PostDecrementExpr(new IdExpr(*$1,yylineno), yylineno); }
    ;

jump_statement: RETU_TK expression {$$ = new ReturnStatement($2, yylineno);}
    ;

for_statement: declaration_statement ';' expression ';' asignation_statement block_statements { $$ = new ForStatement( $1, $3, $5, $6, yylineno);}
    | block_statements {Expr * tr= new BoolExpr( 1, yylineno); $$ = new ForStatement( NULL, tr, NULL, $1, yylineno);}
    | expression block_statements  { $$ = new ForStatement( NULL, $1,NULL , $2, yylineno);}
    ;

methodcall: ID_TK '.' ID_TK '('')'{ $$ = new MethodInvocationExpr(new IdExpr(*$3,yylineno), *(new ExpressionList), yylineno); }
    | ID_TK '.' ID_TK '(' expression_list ')' { $$ = new MethodInvocationExpr(new IdExpr(*$3,yylineno), *$5, yylineno); }
    | ID_TK '(' expression_list ')'{ $$ = new MethodInvocationExpr(new IdExpr(*$1,yylineno), *$3, yylineno); }
    | ID_TK '('')'{ $$ = new MethodInvocationExpr(new IdExpr(*$1,yylineno), *(new ExpressionList), yylineno); }
    | PRINT_TK '(' expression_list ')' { $$=new Print(*$3,yylineno); }
    ;

expression_list: expression_list ',' expression {$$ = $1; $$->push_back($3);}
    | expression { $$ = new ExpressionList; $$->push_back($1);}

if_statement: IF_TK expression block_statements {$$ = new IfStatement($2, $3, yylineno);}
    | IF_TK expression block_statements ELSE_TK statement {$$ = new ElseStatement($2, $3, $5, yylineno);}
    ;

block_statements: '{' statement_list '}'
                    { 
                            DeclarationList * list = new DeclarationList();
                            $$ = new BlockStatement(*$2, *list, yylineno);
                            delete list;
                    }
    ;

type: INT_TK {$$ = INT;}
    | BOOL_TK {$$ = BOOL;}
    | FLOAT_TK {$$ = FLOAT;}
    ;

param_list: param_list ',' param {$$ = $1; $$->push_back($3);}
    | param { $$ = new ParameterList; $$->push_back($1); }
    ;

param: ID_TK type { $$ = new Parameter((Type)$2, new Declarator((Type)$2,*$1,NULL,false, yylineno), false, yylineno); }
    //| ID_TK '[' ']' type { Declarator* cos= new Declarator((Type)$4,$1,NULL,true , yylineno); $$ = new Parameter((Type)$4, cos, true, yylineno); }

expression: expression AND_TK unary_expression { $$ = new LogicalAndExpr($1, $3, yylineno); }
    | expression OR_TK unary_expression { $$ = new LogicalOrExpr($1, $3, yylineno); }
    | expression EQUALS_TK unary_expression { $$ = new EqExpr($1, $3, yylineno); }
    | expression NOT_EQUAL_TK unary_expression { $$ = new NeqExpr($1, $3, yylineno); }
    | unary_expression { $$ = $1; }
    ;

unary_expression: '!' relational_expression {$$ = new UnaryExpr(INCREMENT, $2, yylineno);}
    | relational_expression { $$ = $1; }
    ;

relational_expression: relational_expression '<' additive_expression { $$ = new LtExpr($1, $3, yylineno); }
    | relational_expression LESS_OR_EQUAL_TK additive_expression { $$ = new LteExpr($1, $3, yylineno); }
    | relational_expression '>' additive_expression { $$ = new GtExpr($1, $3, yylineno); }
    | relational_expression GREATER_OR_EQUAL_TK additive_expression { $$ = new GteExpr($1, $3, yylineno); }
    | additive_expression { $$ = $1; }
    ;

additive_expression: additive_expression '+' multiplicative_expression { $$ = new AddExpr($1, $3, yylineno); }
    | additive_expression '-' multiplicative_expression  { $$ = new SubExpr($1, $3, yylineno); }
    | multiplicative_expression { $$ = $1; }
    ;

multiplicative_expression: multiplicative_expression '*' primary_expression { $$ = new MulExpr($1, $3, yylineno); }
    | multiplicative_expression '/' primary_expression { $$ = new DivExpr($1, $3, yylineno); }
    | multiplicative_expression '%' primary_expression { $$ = new ModExpr($1, $3, yylineno); }
    | primary_expression { $$ = $1; }
    ;

primary_expression: '('expression ')' {$$ = $2;}
    | ID_TK {$$ = new IdExpr(*$1, yylineno);}
    //| ID_TK '[' expression ']' 
    | LIT_STRING_TK { $$ = new StringExpr(*$1 , yylineno);}
    | LIT_INT_TK { $$ = new IntExpr($1 , yylineno);}
    | LIT_FLOAT_TK { $$ = new FloatExpr($1 , yylineno);}
    | TRUE_TK { $$ = new BoolExpr( 1, yylineno);}
    | FALSE_TK { $$ = new BoolExpr( 0, yylineno);}
    //| '['']' type '{' expression_list '}' ';' 
    | methodcall { $$ = $1;}
    ;


assignment_operator: '=' { $$ = EQUAL; }
    | PLUS_EQUAL_TK { $$ = PLUSEQUAL; }
    | MINUS_EQUAL_TK { $$ = MINUSEQUAL; }
    | MULT_EQUAL_TK { $$ = MULTEQUAL; }
    | SLASH_EQUAL_TK { $$ = SLASHEQUAL; }
    | PORC_EQUAL_TK { $$ = PORCEQUAL; }
    | TRAN_EQUAL_TK { $$ = TRANEQUAL; }
    ;