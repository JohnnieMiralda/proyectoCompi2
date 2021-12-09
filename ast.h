#ifndef _AST_H_
#define _AST_H_

#include <list>
#include <string>
#include <map>


using namespace std;

class Expr;
class InitDeclarator;
class Declaration;
class Parameter;
class Statement;
typedef list<Expr *> InitializerElementList;
typedef list<InitDeclarator *> InitDeclaratorList;
typedef list<Declaration *> DeclarationList;
typedef list<Parameter *> ParameterList;
typedef list<Statement *> StatementList;
typedef list<Expr *> ArgumentList;


#endif