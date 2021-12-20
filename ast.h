#ifndef _AST_H_
#define _AST_H_

#include <list>
#include <string>
#include <map>
#include "code.h"


using namespace std;

class Expr;
typedef list<Expr *> ExpressionList;
typedef list<Expr *> InitializerElementList;

class Statement;
typedef list<Statement *> StatementList;

class Parameter;
typedef list<Parameter *> ParameterList;

class InitDeclarator;
typedef list<InitDeclarator *> InitDeclaratorList;

class Declarator;
typedef list<Declarator *> DeclarationList;


enum StatementKind{
    WHILE_ST,
    FOR_ST,
    IF_ST,
    ELSE_ST,

    POST_DECREMENT_ST,
    POST_INCREMENT_ST,
    DECLARATION_ST,
    EXPRESSION_ST,
    ASSIGNATION_ST,
    BLOCK_ST,

    RETURN_ST,
    BREAK_ST,
    CONT_ST,

    FUNC_DEFINITION_ST,
    GLOBAL_DECLARATION_ST,
    FUNCTION_DEFINITION_ST
};

enum UnaryType{
    INCREMENT,
    DECREMENT,
    NOT
};

class Statement{
    public:
        int line;
        virtual int evaluateSemantic() = 0;
        virtual StatementKind getKind() = 0;
        virtual string genCode() = 0;
        // virtual string genCode(string label) = 0;
};
class Expr{
    public:
        int line;
        virtual Type getType() = 0;
        virtual void genCode(Code &code) = 0;
};
class IdExpr : public Expr{
    public:
        IdExpr(string id, int line){
            this->id = id;
            this->line = line;
        }
        string id;
        int line;
        Type getType();
        int evaluateSemantic();
        void genCode(Code &code);
};
class AsignationStatement: public Statement{
    public:
        AsignationStatement(IdExpr* id, int operador, Expr * expr){
            this->id = id;
            this->operador= operador;
            this->expr= expr;
        }
        IdExpr * id;
        int operador;
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return ASSIGNATION_ST;
        }
};


class BlockStatement : public Statement{
    public:
        BlockStatement(StatementList statements, DeclarationList declarations, int line){
            this->statements = statements;
            this->declarations = declarations;
            this->line = line;
        }
        StatementList statements;
        DeclarationList declarations;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return BLOCK_ST;
        }
};
class Print: public Expr{
    public:
        Print(ExpressionList args, int line){
            this->args= args;
            this->line= line;
        }
        ExpressionList args;
        int line;
        Type getType();
        void genCode(Code &code);
};

class BreakStatement: public Statement{
    public: 
        BreakStatement(int line){
            this->line = line;
        }
        int line;
        int evaluateSemantic();
        StatementKind getKind(){
            return BREAK_ST;
        };
        string genCode();
};

class ContinueStatement: public Statement{
    public: 
        ContinueStatement(int line){
            this->line=line;
        }
        int line;
        int evaluateSemantic();
        StatementKind getKind(){
            return CONT_ST;
        };
        string genCode();
};

class MethodDefinition : public Statement{
    public:
        MethodDefinition(Type type, string id, ParameterList params, StatementList statement, DeclarationList declarations, int line){
            this->type = type;
            this->id = id;
            this->params = params;
            this->statements = statement;
            this->declarations = declarations;
            this->line = line;
        }

        Type type;
        string id;
        ParameterList params;
        StatementList  statements;
        DeclarationList declarations;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return FUNCTION_DEFINITION_ST;
        }
};


class Initializer{
    public:
        Initializer( Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int line;
};

class Declarator{
    public:
        Declarator(Type type, string id, Initializer * initializer, bool isArray, int line){
            this-> type = type;
            this-> id = id;
            this-> initializer = initializer;
            this-> isArray = isArray;
            this-> line = line;
	    }
        Type type;
        string id;
        Initializer * initializer;
        bool isArray;
        int line;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){
            return DECLARATION_ST;
        }
};

class Parameter{
    public:
        Parameter(Type type, Declarator * declarator, bool isArray, int line){
            this->type = type;
            this->declarator = declarator;
            this->line = line;
        }
        Type type;
        Declarator* declarator;
        bool isArray;
        int line;
        int evaluateSemantic();
};



class IntExpr : public Expr{
    public:
        IntExpr(int value, int line){
            this->value = value;
            this->line = line;
        }
        int value;
        int line;
        Type getType();
        void genCode(Code &code);
};
class FloatExpr : public Expr{
    public:
        FloatExpr(float value, int line){
            this->value = value;
            this->line = line;
        }
        float value;
        Type getType();
        void genCode(Code &code);
};
class BoolExpr : public Expr{
    public:
        BoolExpr(int value, int line){
            this->value = value;
            this->line = line;
        }
        int value;
        Type getType();
        void genCode(Code &code);
};

class StringExpr : public Expr{
    public:
        StringExpr(string value, int line){
            this->value = value;
            this->line = line;
        }
        string value;
        Type getType();
        void genCode(Code &code);
};



class UnaryExpr : public Expr{
    public:
        UnaryExpr(int type, Expr* expr, int line){
            this->type = type;
            this->expr = expr;
            this->line = line;
        }
        int type;
        Expr* expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class PostIncrementExpr: public Statement{
    public:
        PostIncrementExpr(IdExpr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        IdExpr * expr;
        int line;
        Type getType();
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){
            return POST_INCREMENT_ST;
        }
};

class PostDecrementExpr: public Statement{
    public:
        PostDecrementExpr(IdExpr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        IdExpr * expr;
        int line;
        Type getType();
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){
            return POST_DECREMENT_ST;
        }
};

class MethodInvocationExpr : public Expr{
    public:
        MethodInvocationExpr(IdExpr * id, ExpressionList args, int line){
            this->id = id;
            this->args = args;
            this->line = line;
        }
        IdExpr * id;
        ExpressionList args;
        int line;
        Type getType();
        void genCode(Code &code);

};

class ArrayExpr : public Expr{
    public:
        ArrayExpr(IdExpr * id, Expr * expr, int line){
            this->id = id;
            this->expr = expr;
            this->line = line;
        }
        IdExpr * id;
        Expr * expr;
        int line;
        Type getType();
        void genCode(Code &code);
};

class BinaryExpr : public Expr{
    public:
        BinaryExpr(Expr * expr1, Expr *expr2, int line){
            this->expr1 = expr1;
            this->expr2 = expr2;
            this->line = line;
        }
        Expr * expr1;
        Expr *expr2;
        int line;
};

class ElseStatement : public Statement{
    public:
        ElseStatement(Expr * conditionalExpr, BlockStatement * trueStatement, Statement * falseStatement, int line){
            this->conditionalExpr = conditionalExpr;
            this->trueStatement = trueStatement;
            this->line = line;
            this->falseStatement = falseStatement;
        }
        Expr * conditionalExpr;
        BlockStatement * trueStatement;
        Statement * falseStatement;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){return ELSE_ST;}
};

class IfStatement : public Statement{
    public:
        IfStatement(Expr * conditionalExpr, BlockStatement * trueStatement, int line){
            this->conditionalExpr = conditionalExpr;
            this->trueStatement = trueStatement;
            this->line = line;
        }
        Expr * conditionalExpr;
        BlockStatement * trueStatement;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){return IF_ST;}
};

class ForStatement: public Statement{
    public:
        ForStatement(Declarator * decl, Expr * expr,  Statement * asignation, Statement * stmt, int line){
            this->decl = decl;
            this->expr = expr;
            this->asignation = asignation;
            this->stmt = stmt;
            this->line = line;
        }
        Declarator * decl;
        Expr* expr;
        Statement * stmt;
        Statement * asignation;
        int line;
        string genCode();
        int evaluateSemantic();
        StatementKind getKind(){
            return FOR_ST;
        }
};

class ExprStatement : public Statement{
    public:
        ExprStatement(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){return EXPRESSION_ST;}
};

class ReturnStatement : public Statement{
    public:
        ReturnStatement(Expr * expr, int line){
            this->expr = expr;
            this->line = line;
        }
        Expr * expr;
        int evaluateSemantic();
        string genCode();
        StatementKind getKind(){return RETURN_ST;}
};

#define IMPLEMENT_BINARY_EXPR(name) \
class name##Expr : public BinaryExpr{\
    public: \
        name##Expr(Expr * expr1, Expr *expr2, int line) : BinaryExpr(expr1, expr2, line){}\
        Type getType(); \
        void genCode(Code &code);\
};

IMPLEMENT_BINARY_EXPR(Add);
IMPLEMENT_BINARY_EXPR(Sub);
IMPLEMENT_BINARY_EXPR(Mul);
IMPLEMENT_BINARY_EXPR(Div);
IMPLEMENT_BINARY_EXPR(Mod);
IMPLEMENT_BINARY_EXPR(Tra);
IMPLEMENT_BINARY_EXPR(Eq);
IMPLEMENT_BINARY_EXPR(Neq);
IMPLEMENT_BINARY_EXPR(Gte);
IMPLEMENT_BINARY_EXPR(Lte);
IMPLEMENT_BINARY_EXPR(Gt);
IMPLEMENT_BINARY_EXPR(Lt);
IMPLEMENT_BINARY_EXPR(LogicalAnd);
IMPLEMENT_BINARY_EXPR(LogicalOr);
IMPLEMENT_BINARY_EXPR(Assign);
IMPLEMENT_BINARY_EXPR(PlusEqual);
IMPLEMENT_BINARY_EXPR(MinusEqual);
IMPLEMENT_BINARY_EXPR(TimesEqual);
IMPLEMENT_BINARY_EXPR(ExponentEqual);
IMPLEMENT_BINARY_EXPR(DivideEqual);
IMPLEMENT_BINARY_EXPR(ModEqual);
#endif