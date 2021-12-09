#include "ast.h"
#include <map>
#include <list>
#include <iostream>


map<string,Expr*> globalVariables;
map<string,Expr*> methods;

float AssigExpr::evaluate(){
    if(globalVariables.empty()){
        globalVariables[this->name]=this->exp;
        return 1;
    }else if(globalVariables[this->name]!=0){
        globalVariables[this->name]=this->exp;
        return 0;
    }else{
        globalVariables[this->name]=this->exp;
        return 1;
    }
}

float AssigMethExpr::evaluate(){
    if(methods.empty()){
        methods[this->name]=this->exp;
        return 1;
    }else if(methods[this->name]!=0){
        return 0;
    }else{
        methods[this->name]=this->exp;
        return 1;
    }
}
float CallExpr::evaluate(){
    if(globalVariables[this->name]!=0){
        return globalVariables[this->name]->evaluate();
    }else{
        printf("variable no existe\n");
        return -1;
    }
}

float CallMethExpr::evaluate(){
    if(methods[this->name]!=0){
        return methods[this->name]->evaluate();
    }else{
        printf("method no existe\n");
        return -1;
    }
}

float GreaterExpr::evaluate(){
    return this->exp1->evaluate() < this->exp2->evaluate();
}
float LessExpr::evaluate(){
    return this->exp1->evaluate() > this->exp2->evaluate();
}

float NumExpr::evaluate(){
    return this->num;
}
float AddExpr::evaluate(){
    return this->exp1->evaluate() + this->exp2->evaluate();
}
float SubExpr::evaluate(){
    return this->exp1->evaluate() - this->exp2->evaluate();
}
float MulExpr::evaluate(){
    return this->exp1->evaluate() * this->exp2->evaluate();
}
float DivExpr::evaluate(){
    return this->exp1->evaluate() / this->exp2->evaluate();
}