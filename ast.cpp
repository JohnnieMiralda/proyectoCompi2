#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
// #include "asm.h"

// extern Asm assemblyFile;

int globalStackPointer = 0;

class VariableInfo{
    public:
        VariableInfo(int offset, bool isArray, bool isParameter, Type type){
            this->offset = offset;
            this->isArray = isArray;
            this->isParameter = isParameter;
            this->type = type;
        }
        int offset;
        bool isArray;
        bool isParameter;
        Type type;
};

map<string, VariableInfo *> codeGenerationVars;
map<string, bool> initilizedVariables;

class ContextStack{
    public:
        struct ContextStack* prev;
        map<string, Type> variables;
};

class FunctionInfo{
    public:
        Type returnType;
        list<Parameter *> parameters;
};

int labelCounter = 0;
map<string, Type> globalVariables = {};
map<string, Type> variables;
map<string, FunctionInfo*> methods;
map<string, Type> resultTypes ={
    {"INT,INT", INT},
    {"FLOAT,FLOAT", FLOAT},
    {"INT,FLOAT", FLOAT},
    {"FLOAT,INT", FLOAT},
};

const char * intTemps[] = {"$t0","$t1", "$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9" };
const char * floatTemps[] = {"$f0","$f1","$f2","$f3","$f4","$f5","$f6","$f7","$f8","$f9","$f10","$f11",
                            "$f12","$f13","$f14","$f15","$f16","$f17","$f18","$f19","$f20","$f21","$f22",
                            "$f23","$f24","$f25","$f26","$f27","$f28","$f29","$f30","$f31"};

#define INT_TEMP_COUNT 10
#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

string getIntTemp(){
    for (int i = 0; i < INT_TEMP_COUNT; i++)
    {
        if(intTempMap.find(intTemps[i]) == intTempMap.end()){
            intTempMap.insert(intTemps[i]);
            return string(intTemps[i]);
        }
    }
    return "";
}

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    return "";
}

void releaseIntTemp(string temp){
    intTempMap.erase(temp);
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

void releaseRegister(string temp){
    releaseIntTemp(temp);
    releaseFloatTemp(temp);
}

string getTypeName(Type type){
    switch(type){
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case VOID:
            return "VOID";
        case INT_ARRAY:
            return "INT";
        case FLOAT_ARRAY:
            return "FLOAT";
        case BOOL:
            return "BOOL";
    }

    cout<<"Unknown type"<<endl;
    exit(0);
}

ContextStack * context = NULL;

void pushContext(){
    variables.clear();
    ContextStack * c = new ContextStack();
    c->variables = variables;
    c->prev = context;
    context = c;
}

void popContext(){
    if(context != NULL){
        ContextStack * previous = context->prev;
        free(context);
        context = previous;
    }
}

string getNewLabel(string prefix){
    stringstream ss;
    ss<<prefix << labelCounter;
    labelCounter++;
    return ss.str();
}

Type getLocalVariableType(string id){
    ContextStack * currContext = context;
    while(currContext != NULL){
        if(currContext->variables[id] != 0)
            return currContext->variables[id];
        currContext = currContext->prev;
    }
    if(!context->variables.empty())
        return context->variables[id];
    return INVALID;
}


Type getVariableType(string id){
    if(!globalVariables.empty())
        return globalVariables[id];
    return INVALID;
}


bool variableExists(string id){
    Type value;
    if(context != NULL){
        value = getLocalVariableType(id);
        //context->variables[id] != 0
        if(value != 0)
        return true;
    }
    return false;
}

int BlockStatement::evaluateSemantic(){
    list<Declarator *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declarator * dec = *itd;
        if(dec != NULL){
            dec->evaluateSemantic();
        }
        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            stmt->evaluateSemantic();
        }

        its++;
    }

    return 0;
}

int ContinueStatement::evaluateSemantic(){
    return 0;
}

string ContinueStatement::genCode(){
    stringstream ss;
    // ss<<"j "<<label;
    return ss.str();
}

int BreakStatement::evaluateSemantic(){
    return 0;
}

string BreakStatement::genCode(){
    stringstream ss;
    // ss<<"j "<<label;
    return ss.str();
}

string BlockStatement::genCode(){
    stringstream ss;

    list<Declarator *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declarator * dec = *itd;
        if(dec != NULL){
            ss<<dec->genCode()<<endl;
        }

        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            if(stmt->getKind()== BREAK_ST || stmt->getKind()== CONT_ST){
                // ss<<stmt->genCode(label)<<endl;
            }else{
                ss<<stmt->genCode()<<endl;
            }
        }

        its++;
    }
    return ss.str();
}

void addMethodDeclaration(string id, int line, Type type, ParameterList params){
    if(methods[id] != 0){
        cout<<"redefinition of function "<<id<<" in line: "<<line<<endl;
        exit(0);
    }
    methods[id] = new FunctionInfo();
    methods[id]->returnType = type;
    methods[id]->parameters = params;
}

Type Print::getType(){
    return VOID;
}

void Print::genCode(Code &code){

}

int MethodDefinition::evaluateSemantic(){
    if(this->params.size() > 4){
        cout<< "Method: "<<this->id << " can't have more than 4 parameters, line: "<< this->line<<endl;
        exit(0);
    }

    addMethodDeclaration(this->id, this->line, this->type, this->params);
    pushContext();

    list<Parameter* >::iterator it = this->params.begin();
    while(it != this->params.end()){
        (*it)->evaluateSemantic();
        it++;
    }

    list<Declarator *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declarator * dec = *itd;
        cout<<endl<<dec->id<<" en chequeo"<<endl;
        if(dec != NULL){
            dec->evaluateSemantic();
        }
        itd++;
    }

    // this->statements->evaluateSemantic();
    list<Statement* >::iterator its = this->statements.begin();
    while(its != this->statements.end()){
        Statement * stmt = *its;
        if(stmt != NULL){
            cout<<"kind"<<(StatementKind)stmt->getKind()<<endl;
            int cosa=stmt->evaluateSemantic();
            if( stmt->getKind() == RETURN_ST ){
                cout<<"en return "<<(Type)cosa<<endl;
                if(Type(cosa) != this->type ){
                    cout<<"Error return value does not match function definition type "<<endl;
                    exit(0);
                }
            }
        }
        its++;
    }

    popContext();

    return 0;
}
string saveState(){
    set<string>::iterator it = floatTempMap.begin();
    stringstream ss;
    ss<<"sw $ra, " <<globalStackPointer<< "($sp)\n";
    globalStackPointer+=4;
    return ss.str();
}
string retrieveState(string state){
    std::string::size_type n = 0;
    string s = "sw";
    while ( ( n = state.find( s, n ) ) != std::string::npos )
    {
    state.replace( n, s.size(), "lw" );
    n += 2;
    globalStackPointer-=4;
    }
    return state;
}
string MethodDefinition::genCode(){
    

    int stackPointer = 4;
    globalStackPointer = 0;
    stringstream code;

    code << this->id<<": "<<endl;
    string state = saveState();
    code <<state<<endl;
    if(this->params.size() > 0){
        list<Parameter*>::iterator it = this->params.begin();
        for(int i = 0; i< this->params.size(); i++){
            Parameter * pm= *it;
            code << "sw $a"<<i<<", "<< stackPointer<<"($sp)"<<endl;
            codeGenerationVars[pm->declarator->id] = new VariableInfo(stackPointer, false, true, pm->type);
            stackPointer +=4;
            globalStackPointer +=4;
            it++;
        }
    }

    list<Declarator *>::iterator itd = this->declarations.begin();
    while (itd != this->declarations.end())
    {
        Declarator * dec = *itd;
        if(dec != NULL){
            code<<dec->genCode()<<endl;
        }

        itd++;
    }

    list<Statement *>::iterator its = this->statements.begin();
    while (its != this->statements.end())
    {
        Statement * stmt = *its;
        if(stmt != NULL){
            if(stmt->getKind()== BREAK_ST || stmt->getKind()== CONT_ST){
                // ss<<stmt->genCode(label)<<endl;
            }else{
                code<<stmt->genCode()<<endl;
            }
        }

        its++;
    }
    stringstream sp;
    int currentStackPointer = globalStackPointer;
    sp << endl<<"addiu $sp, $sp, -"<<currentStackPointer<<endl;
    code << retrieveState(state);
    code << "addiu $sp, $sp, "<<currentStackPointer<<endl;
    code <<"jr $ra"<<endl;
    codeGenerationVars.clear();
    string result = code.str();
    result.insert(id.size() + 2, sp.str());
    return result;
}

Type IntExpr::getType(){
    return INT;
}

void IntExpr::genCode(Code &code){
    string temp = getIntTemp();
    code.place = temp;
    stringstream ss;
    ss << "li " << temp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = INT;
}

Type FloatExpr::getType(){
    return FLOAT;
}

void FloatExpr::genCode(Code &code){
    string floatTemp = getFloatTemp();
    code.place = floatTemp;
    stringstream ss;
    ss << "li.s " << floatTemp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = FLOAT;
}

Type BoolExpr::getType(){
    return BOOL;
}

void BoolExpr::genCode(Code &code){
    string IntTemp = getIntTemp();
    code.place = IntTemp;
    stringstream ss;
    ss << "li " << IntTemp <<", "<< this->value <<endl;
    code.code = ss.str();
    code.type = BOOL;
}

#define IMPLEMENT_BINARY_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypes[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: type "<< leftType <<" can't be converted to type "<< rightType <<" line: "<<this->line<<endl;\
        exit(0);\
    }\
    return resultType; \
}\

void toFloat(Code &code){
    if(code.type == INT){
        string floatTemp = getFloatTemp();
        stringstream ss;
        ss << code.code
        << "mtc1 "<< code.place << ", " << floatTemp <<endl
        << "cvt.s.w " << floatTemp<< ", " << floatTemp <<endl;
        releaseRegister(code.place);
        code.place = floatTemp;
        code.type = FLOAT;
        code.code =  ss.str();
    }
    else{
        /* nothing */
    }
}

#define IMPLEMENT_BINARY_ARIT_GEN_CODE(name, op)\
void name##Expr::genCode(Code &code){\
    Code leftCode, rightCode;\
    stringstream ss;\
    this->expr1->genCode(leftCode);\
    this->expr2->genCode(rightCode);\
    if(leftCode.type == INT && rightCode.type == INT){\
        code.type = INT;\
        releaseRegister(leftCode.place);\
        releaseRegister(rightCode.place);\
        ss<< leftCode.code << endl\
        << rightCode.code <<endl\
        << intArithmetic(leftCode, rightCode, code, op)<< endl;\
    }else{\
        code.type = FLOAT;\
        toFloat(leftCode);\
        toFloat(rightCode);\
        releaseRegister(leftCode.place);\
        releaseRegister(rightCode.place);\
        ss << leftCode.code << endl\
        << rightCode.code <<endl\
        << floatArithmetic(leftCode, rightCode, code, op)<<endl;\
    }\
    code.code = ss.str();\
}\

#define IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(name)\
Type name##Expr::getType(){\
    string leftType = getTypeName(this->expr1->getType());\
    string rightType = getTypeName(this->expr2->getType());\
    Type resultType = resultTypes[leftType+","+rightType];\
    if(resultType == 0){\
        cerr<< "Error: type "<< leftType <<" can't be converted to type "<< rightType <<" line: "<<this->line<<endl;\
        exit(0);\
    }\
    return BOOL; \
}\


string intArithmetic(Code &leftCode, Code &rightCode, Code &code, char op){
    stringstream ss;
    code.place = getIntTemp();
    switch (op)
    {
        case '+':
            ss << "add "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '-':
            ss << "sub "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '*':
            ss << "mult "<< leftCode.place <<", "<< rightCode.place <<endl
            << "mflo "<< code.place;
            break;
        case '/':
            ss << "div "<< leftCode.place <<", "<< rightCode.place
            <<endl<< "mflo "<< code.place;
            break;
        case '%':
            ss << "div "<< leftCode.place <<", "<< rightCode.place
            <<endl<< "mfhi "<< code.place;
            break;
        default:
            break;
    }
    return ss.str();
}

string floatArithmetic(Code &leftCode, Code &rightCode, Code &code, char op){
    stringstream ss;
    code.place = getFloatTemp();
    switch (op)
    {
        case '+':
            ss << "add.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '-':
            ss << "sub.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '*':
            ss << "mul.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        case '/':
            ss << "div.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
            break;
        default:
            break;
    }
    return ss.str();
}

Type getUnaryType(Type expressionType, int unaryOperation){
    switch(unaryOperation){
        case INCREMENT:
        case DECREMENT:
            if(expressionType == INT || expressionType == FLOAT)
                return expressionType;
        case NOT:
            if(expressionType == BOOL)
                return BOOL;
    }

    cerr<<"Error: Invalid type"<<endl;
    exit(0);
}

int Parameter::evaluateSemantic(){
    if(!variableExists(this->declarator->id)){
        context->variables[declarator->id] = this->type;
    }else{
        cout<<"error: redefinition of variable: "<< declarator->id<< " line: "<<this->line <<endl;
        exit(0);
    }
    return 0;
}


Type UnaryExpr::getType(){
    return this->expr->getType();
}

void UnaryExpr::genCode(Code &code){
    Code exprCode;
    stringstream ss;

    ss<<code.code<<endl;

    this->expr->genCode(exprCode);
    ss<<exprCode.code;

    if(exprCode.type == BOOL){
        ss<<"not "<< exprCode.place << " , " << exprCode.place<<endl;
        code.code= ss.str();
        code.type= exprCode.type;
        code.place= exprCode.place;
    }else{
        code.code=ss.str();
    }
}

Type ArrayExpr::getType(){
    return this->id->getType();
}

void ArrayExpr::genCode(Code &code){
    Code arrayCode;
    string name = this->id->id;
    stringstream ss;
    this->expr->genCode(arrayCode);
    //a[1]
    if (codeGenerationVars.find(name) == codeGenerationVars.end())
    {
        string temp = getIntTemp();
        string labelAddress = getIntTemp();
        ss << arrayCode.code<<endl
        << "li $a0, 4"<<endl
        << "mult $a0, "<< arrayCode.place<<endl
        <<"mflo "<<temp<<endl
        << "la "<< labelAddress<<", "<< name<<endl
        << "add "<<temp<<", "<<labelAddress<<", "<<temp<<endl;
        releaseRegister(arrayCode.place);
        releaseRegister(labelAddress);
        if(globalVariables[name] == INT_ARRAY){
           ss <<"lw "<< temp<<", 0("<<temp<<")"<<endl;
           code.place = temp;
        }else{
            string floatTemp = getFloatTemp();
            ss <<"l.s "<< floatTemp<<", 0("<<temp<<")"<<endl;
           code.place = floatTemp;
        }
    }else{
        string temp = getIntTemp();
        string address = getIntTemp();
        ss << arrayCode.code<<endl
        << "li $a0, 4"<<endl
        << "mult $a0, "<< arrayCode.place<<endl
        <<"mflo "<<temp<<endl
        << "la "<<address<<", "<<codeGenerationVars[name]->offset<<"($sp)"<<endl
        << "add "<<temp<<", "<<address<<", "<<temp<<endl;
        if(codeGenerationVars[name]->type == INT_ARRAY){
           ss <<"lw "<< temp<<", 0("<<temp<<")"<<endl;
           code.place = temp;
           code.type = INT;
        }else{
            string floatTemp = getFloatTemp();
            ss <<"l.s "<< floatTemp<<", 0("<<temp<<")"<<endl;
           code.place = floatTemp;
           code.type = FLOAT;
        }
    }
    code.code = ss.str();
}

Type IdExpr::getType(){
    Type value;
    if(context != NULL){
        value = getLocalVariableType(this->id);
        if(value != 0)
            return value;
    }
    value = getVariableType(this->id);
    if(value == 0){
        cout<<"error: '"<<this->id<<"' was not declared in this scope line: "<<this->line<<endl;
        exit(0);
    }
    return value;
}

void IdExpr::genCode(Code &code){
    if(codeGenerationVars.find(this->id) == codeGenerationVars.end()){
        code.type = globalVariables[this->id];
        if(globalVariables[this->id] == INT){
            string intTemp = getIntTemp();
            code.place = intTemp;
            code.code = "lw "+ intTemp + ", "+ this->id + "\n";
        }else if(globalVariables[this->id] == FLOAT){
            string floatTemp = getFloatTemp();
            code.place = floatTemp;
            code.code = "l.s "+ floatTemp + ", "+ this->id + "\n";
        }
    }
   else{
        code.type = codeGenerationVars[this->id]->type;
        if(codeGenerationVars[this->id]->type == INT && !codeGenerationVars[this->id]->isArray){
            string intTemp = getIntTemp();
            code.place = intTemp;
            code.code = "lw "+ intTemp + ", " + to_string(codeGenerationVars[this->id]->offset) +"($sp)\n";
        }else if(codeGenerationVars[this->id]->type == FLOAT && !codeGenerationVars[this->id]->isArray){
            string floatTemp = getFloatTemp();
            code.place = floatTemp;
            code.code = "l.s "+ floatTemp + ", " +to_string(codeGenerationVars[this->id]->offset) +"($sp)\n";
        }else if(codeGenerationVars[this->id]->type == BOOL && !codeGenerationVars[this->id]->isArray){
            string boolTemp = getIntTemp();
            code.place = boolTemp;
            code.code = "lw "+ boolTemp + ", " +to_string(codeGenerationVars[this->id]->offset) +"($sp)\n";
        }
    }
}

Type MethodInvocationExpr::getType(){
    FunctionInfo * func = methods[this->id->id];
    if(func == NULL){
        cout<<"error: function "<<this->id->id<<" not found, line: "<<this->line<<endl;
        exit(0);
    }
    Type funcType = func->returnType;
    if(func->parameters.size() > this->args.size()){
        cout<<"error: to few arguments to function"<<this->id->id<<" line: "<<this->line<<endl;
        exit(0);
    }
    if(func->parameters.size() < this->args.size()){
        cout<<"error: to many arguments to function "<<this->id->id<<" line: "<<this->line<<endl;
        exit(0);
    }

    list<Parameter *>::iterator paramIt = func->parameters.begin();
    list<Expr *>::iterator argsIt =this->args.begin();
    while(paramIt != func->parameters.end() && argsIt != this->args.end()){
        string paramType = getTypeName((*paramIt)->type);
        string argType = getTypeName((*argsIt)->getType());
        if( paramType != argType){
            cout<<"error: invalid conversion from: "<< argType <<" to " <<paramType<< " line: "<<this->line <<endl;
            exit(0);
        }
        paramIt++;
        argsIt++;
    }

    return funcType;
}

void MethodInvocationExpr::genCode(Code &code){
    list<Expr *>::iterator it = this->args.begin();
    list<Code> codes;
    stringstream ss;
    Code argCode;
    while (it != this->args.end())
    {
        (*it)->genCode(argCode);
        ss << argCode.code <<endl;
        codes.push_back(argCode);
        it++;
    }

    int i = 0;
    list<Code>::iterator placesIt = codes.begin();
    while (placesIt != codes.end())
    {
        releaseRegister((*placesIt).place);
        if((*placesIt).type == INT)
            ss << "move $a"<<i<<", "<< (*placesIt).place<<endl;
        else if((*placesIt).type == FLOAT)
            ss << "mfc1 $a"<<i<<", "<< (*placesIt).place<<endl;
        else if((*placesIt).type == BOOL)
            ss << "move $a"<<i<<", "<< (*placesIt).place<<endl;
        i++;
        placesIt++;
    }

    ss<< "jal "<< this->id->id<<endl;
    string reg;
    if(methods[this->id->id]->returnType == INT){
        reg = getIntTemp();
        ss << "move "<< reg<<", $v0";
    }
    else if(methods[this->id->id]->returnType == FLOAT){
        reg = getFloatTemp();
        ss << "mfc1 $v0, "<< reg<<endl;
    }
    else if(methods[this->id->id]->returnType == BOOL){
        reg = getIntTemp();
        ss << "move "<< reg<<", $v0";
    }
    code.code = ss.str();
    code.place = reg;
}

Type PostIncrementExpr::getType(){
    return this->expr->getType();
}

string PostIncrementExpr::genCode(){
    stringstream ss;
    // this->expr->evaluateSemantic();
    Type exprType = this->expr->getType();
    Code code;
    if (exprType == INT){
        this->expr->genCode(code);
        ss<<code.code<<endl;
        ss<<"addi "<< code.place <<" , "<< code.place << " , 1 "<<endl;
        ss<<"sw "<< code.place << "," << to_string(codeGenerationVars[this->expr->id]->offset) << "($sp)"<<endl;
        releaseRegister(code.place);
        return ss.str();
    }else{
        this->expr->genCode(code);
        string floattemp= getFloatTemp();
        ss<<code.code<<endl;
        ss<< "li.s" << floattemp <<" , 1.0"<<endl;
        ss<<"add.s "<< code.place <<" , "<< code.place << " , "<<  floattemp <<endl;
        ss<<"s.s "<< code.place << "," << to_string(codeGenerationVars[this->expr->id]->offset) << "($sp)"<<endl;
        releaseRegister(code.place);
        releaseRegister(floattemp);
        return ss.str();
    }
    return "";
}

int PostIncrementExpr::evaluateSemantic(){
    Type exprType = this->expr->getType();
        if(exprType != FLOAT && exprType != INT){
            cout<<"error: invalid conversion from: "<< getTypeName(exprType) <<" to line: "<<this->line <<endl;
            exit(0);
        }
    return 0;
}

int PostDecrementExpr::evaluateSemantic(){
    Type exprType = this->expr->getType();
        if(exprType != FLOAT && exprType != INT){
            cout<<"error: invalid conversion from: "<< getTypeName(exprType) <<" to line: "<<this->line <<endl;
            exit(0);
        }
    return 0;
}
Type PostDecrementExpr::getType(){
    return this->expr->getType();
}

string PostDecrementExpr::genCode(){
    stringstream ss;
    // this->expr->evaluateSemantic();
    Type exprType = this->expr->getType();
    Code code;
    if (exprType == INT){
        this->expr->genCode(code);
        ss<<code.code<<endl;
        ss<<"addi "<< code.place <<" , "<< code.place << " , -1 "<<endl;
        ss<<"sw "<< code.place << "," << to_string(codeGenerationVars[this->expr->id]->offset) << "($sp)"<<endl;
        releaseRegister(code.place);
    }else{
        this->expr->genCode(code);
        string floattemp= getFloatTemp();
        ss<<code.code<<endl;
        ss<< "li.s" << floattemp <<" , 1.0"<<endl;
        ss<<"sub.s "<< code.place <<" , "<< code.place << " , "<<  floattemp <<endl;
        ss<<"s.s "<< code.place << "," << to_string(codeGenerationVars[this->expr->id]->offset) << "($sp)"<<endl;
        releaseRegister(code.place);
        releaseRegister(floattemp);
    }
    return "";
}

Type StringExpr::getType(){
    return STRING;
}

void StringExpr::genCode(Code &code){
    string strLabel = getNewLabel("string");
    stringstream ss;
    ss << strLabel <<": .asciiz" << this->value << ""<<endl;
    // assemblyFile.data += ss.str(); 
    code.code = "";
    code.place = strLabel;
    code.type = STRING;
}



int Declarator::evaluateSemantic(){
    cout<<this->id<< " "<<this->line;
    
    if(!variableExists(this->id)){
            context->variables[this->id] = this->type;
    }else{
        cout<<"error: redefinition of variable: "<< this->id<< " line: "<<this->line <<endl;
        exit(0);
    }
    if(this->initializer != NULL){  
        Type exprType = this->initializer->expr->getType();
        if(exprType != FLOAT && exprType != INT && exprType != BOOL){
            cout<<"error: invalid conversion from: "<< getTypeName(exprType) <<" to " <<getTypeName(this->type)<< " line: "<<this->line <<endl;
            exit(0);
        }
        initilizedVariables[this->id]=true;
    }else{
        initilizedVariables[this->id]=false;
    }
    return 0;
}

string Declarator::genCode(){
    stringstream ss;
    Code code;
    if( this->initializer != NULL){
        this->initializer->expr->genCode(code);
        ss<<code.code<<endl;
        if(!this->isArray){
            codeGenerationVars[this->id]= new VariableInfo(globalStackPointer, false, false, this->type);
            globalStackPointer+=4;
            if(this->type != FLOAT){
                ss<<" sw "<< code.place << " , " << codeGenerationVars[this->id]->offset << "($sp)"<<endl;
            }else{
                ss<<" s.s "<< code.place << " , " << codeGenerationVars[this->id]->offset << "($sp)"<<endl;
            }
            releaseRegister(code.place);
        }else{
            //logica del array
        }
    }else{
        if(!this->isArray){
            codeGenerationVars[this->id]= new VariableInfo(globalStackPointer, false, false, this->type);
            globalStackPointer+=4;
            if(this->type != FLOAT){
                string tempint= getIntTemp();
                ss<< "li " << tempint << " , $zero" <<endl;
                ss<<" sw "<< tempint << " , " << codeGenerationVars[this->id]->offset << "($sp)"<<endl;
                releaseRegister(tempint);
            }
            else{
                string tempFloat= getFloatTemp();
                ss<< "li.s " << tempFloat << " , $zero" <<endl;
                ss<<" s.s "<< tempFloat << " , " << codeGenerationVars[this->id]->offset << "($sp)"<<endl;
                releaseRegister(tempFloat);
            }
            
        }
    }

    return ss.str();
}

int ForStatement::evaluateSemantic(){
    
    if(this->asignation != NULL){
        this->asignation->evaluateSemantic();
    }
    if( this->expr != NULL){
        
        if(this->expr->getType() != BOOL){
            cout<<"Expression for while must be boolean";
            exit(0);
        }
    }
    

    pushContext();
    if(this->decl != NULL){
        this->decl->evaluateSemantic();
    }
    if(this->stmt != NULL){
        this->stmt->evaluateSemantic();
    }
    popContext();
    return 0;
}

int AsignationStatement::evaluateSemantic(){
    
    if( this->id->getType() != BOOL  ){
        if(variableExists(this->id->id)){

            if( this->expr->getType() == this->id->getType()){
                return 0;
            }else{
                cout<<"error: variable: "<< this->id->id << " does not match type" << " line: "<<this->id->line <<endl;
                exit(0);
            }
        }
        else{
            cout<<"error: variable: "<< " line: "<<this->id->line <<endl;
            exit(0);
        }
    }else{
        if(this->operador==1){
            if(variableExists(this->id->id)){
                if( this->expr->getType() == this->id->getType()){
                    return 0;
                }else{
                    cout<<"error: variable: "<< this->id->id << " does not match type" << " line: "<<this->id->line <<endl;
                    exit(0);
                }
            }
            else{
                cout<<"error: variable: "<< " line: "<<this->id->line <<endl;
                exit(0);
            }
        }else{
            cout<<"error: variable: "<< this->id->id << "does not support this operration statement line: "<<this->id->line <<endl;
            exit(0);
        }
    }

    
    return 0;
}

string AsignationStatement::genCode(){
    stringstream ss;
    Code code;
    this->expr->genCode(code);
    ss<<code.code<<endl;
    if(code.type != FLOAT){
        string tempint= getIntTemp();
        switch ((int)this->operador)
        {
        case 1:
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 2:
            ss<< "lw "<< tempint << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "add "<< code.place << " , " << code.place << " , "<< tempint <<endl;
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 3:
            ss<< "lw "<< tempint << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "sub "<< code.place << " , " << code.place << " , "<< tempint <<endl;
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 4:
            ss<< "lw "<< tempint << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "mul "<< code.place << " , " << code.place << " , "<< tempint <<endl;
            // ss<< "mult "<< code.place << " , "<< tempint <<endl;
            // ss<< "mflo "<< code.place << endl;
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 5:
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 6:
            ss<< "lw "<< tempint << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "div "<< code.place << " , "<< tempint <<endl;
            ss<< "mfhi "<< code.place << endl;
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 7:
            ss<< "lw "<< tempint << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "div "<< code.place << " , "<< tempint <<endl;
            ss<< "mflo "<< code.place << endl;
            ss<< "sw "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        default:
            break;
        }
        releaseRegister(tempint);
        releaseRegister(code.place);
    }else{
        string tempFloat= getFloatTemp();
        switch ((int) this->operador)
        {
        case 1:
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 2:
            ss<< "l.s "<< tempFloat << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "add.s "<< code.place << " , " << code.place << " , "<< tempFloat <<endl;
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 3:
            ss<< "l.s "<< tempFloat << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "sub.s "<< code.place << " , " << code.place << " , "<< tempFloat <<endl;
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 4:
            ss<< "l.s "<< tempFloat << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "mul.s "<< code.place << " , " << code.place << " , "<< tempFloat <<endl;
            // ss<< "mult "<< code.place << " , "<< tempint <<endl;
            // ss<< "mflo "<< code.place << endl;
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 5:
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 6:
            ss<< "l.s "<< tempFloat << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "div.s "<< code.place << " , "<< code.place << " , "<< tempFloat <<endl;
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        case 7:
            ss<< "l.s "<< tempFloat << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            ss<< "div.s "<< code.place << " , "<< code.place << " , "<< tempFloat <<endl;
            ss<< "s.s "<< code.place << " , " << codeGenerationVars[this->id->id]->offset << "($sp)"<<endl;
            break;
        default:
            break;
        }
        releaseRegister(tempFloat);
        releaseRegister(code.place);
    }
    return ss.str();
}

string ForStatement::genCode(){
    stringstream ss;
    string forLabel = getNewLabel("for");
    string endForLabel = getNewLabel("endFor");
    Code code;

    if (this->decl != NULL){
        ss<<this->decl->genCode();
    }

    this->expr->genCode(code);
    ss << forLabel << ": "<< endl
    << code.code <<endl;
    if(code.type == INT || code.type == BOOL){
        ss<< "beqz "<< code.place << ", " << endForLabel <<endl;
    }else{
        ss << "bc1f "<< endForLabel <<endl;
    }
    if( this->stmt->getKind()== BREAK_ST || CONT_ST){
        // ss<< this->stmt->genCode(endForLabel) <<endl;
    }else{
        ss<< this->stmt->genCode() <<endl;
    }

    if (this->asignation != NULL){
        ss<<this->asignation->genCode();
    }

    ss<< "j " << forLabel <<endl;
    ss<< endForLabel << ": "<<endl;
    return ss.str();
}

int ElseStatement::evaluateSemantic(){
    if(this->conditionalExpr->getType() != BOOL){
        cout<<"Expression for if must be boolean";
        exit(0);
    }
    pushContext();
    this->trueStatement->evaluateSemantic();
    popContext();
    pushContext();
    if(this->falseStatement != NULL)
        this->falseStatement->evaluateSemantic();
    popContext();
    return 0;
}

string ElseStatement::genCode(){
    string elseLabel = getNewLabel("else");
    string endIfLabel = getNewLabel("endif");
    Code exprCode;
    this->conditionalExpr->genCode(exprCode);
    stringstream code;
    code << exprCode.code << endl;
    if(exprCode.type == INT || exprCode.type == BOOL){
        code<< "beqz "<< exprCode.place << ", " << elseLabel <<endl;
    }else{
        code << "bc1f "<< elseLabel <<endl;
    }
    code << this->trueStatement->genCode() << endl
    << "j " <<endIfLabel << endl
    << elseLabel <<": " <<endl;
    if(this->falseStatement->getKind()==BLOCK_ST || this->falseStatement->getKind()==CONT_ST){
        // code<< this->falseStatement->genCode(endIfLabel) <<endl;
    }else{
        code<< this->falseStatement->genCode() <<endl;
    }
    code<< endIfLabel <<" :"<< endl;
    releaseRegister(exprCode.place);
    return code.str();
}

int IfStatement::evaluateSemantic(){
    if(this->conditionalExpr->getType() != BOOL){
        cout<<"Expression for if must be boolean";
        exit(0);
    }
    pushContext();
    this->trueStatement->evaluateSemantic();
    popContext();
    return 0;
}

string IfStatement::genCode(){
    string endIfLabel = getNewLabel("endif");
    Code exprCode;
    this->conditionalExpr->genCode(exprCode);
    stringstream code;
    code << exprCode.code << endl;
    if(exprCode.type == INT || exprCode.type == BOOL){
        code<< "beqz "<< exprCode.place << ", " << endIfLabel <<endl;
    }else{
        code << "bc1f "<< endIfLabel <<endl;
    }
    code<< this->trueStatement->genCode() << endl
    << endIfLabel <<" :"<< endl;
    releaseRegister(exprCode.place);
    
    return code.str();
}

int ExprStatement::evaluateSemantic(){
    return this->expr->getType();
}

string ExprStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    return exprCode.code;
}

int ReturnStatement::evaluateSemantic(){
    return this->expr->getType();
}

string ReturnStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    stringstream ss;
    if(exprCode.type != FLOAT){
        ss << exprCode.code << endl
        << "move $v0, "<< exprCode.place <<endl;
    }else{
        ss << exprCode.code << endl
        << "move $v0, "<< exprCode.place <<endl;
    }    
    return ss.str();
}

// int PrintStatement::evaluateSemantic(){
//     return this->expr->getType();
// }

// string PrintStatement::genCode(){
//     Code exprCode;
//     this->expr->genCode(exprCode);
//     stringstream code;
//     code<< exprCode.code<<endl;
//     if(exprCode.type == INT){
//         code <<"move $a0, "<< exprCode.place<<endl
//         << "li $v0, 1"<<endl
//         << "syscall"<<endl;
//     }else if(exprCode.type == FLOAT){
//         code << "mov.s $f12, "<< exprCode.place<<endl
//         << "li $v0, 2"<<endl
//         << "syscall"<<endl;
//     }else if(exprCode.type == STRING){
//         code << "la $a0, "<< exprCode.place<<endl
//         << "li $v0, 4"<<endl
//         << "syscall"<<endl;
//     }
//     return code.str();
// }

void EqExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    stringstream ss;
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< leftSideCode.code <<endl
        << rightSideCode.code <<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        string label = getNewLabel("label");
        string finalLabel = getNewLabel("finalLabel");
        ss << "beq " << leftSideCode.place << ", " << rightSideCode.place << ", " << label + "\n";
        ss << "addi " << temp << ", $zero, 0"<<endl << " j " << finalLabel <<endl;
        ss<< label <<":"<<endl<< "addi " << temp << ", $zero, 1"<<endl<<finalLabel<<":"<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss << leftSideCode.code << endl
        << rightSideCode.code <<endl;
    }
    code.code = ss.str();
}

void NeqExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    stringstream ss;
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< leftSideCode.code <<endl
        << rightSideCode.code <<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        string label = getNewLabel("label");
        string finalLabel = getNewLabel("finalLabel");
        ss << "bne " << leftSideCode.place << ", " << rightSideCode.place << ", " << label + "\n";
        ss << "addi " << temp << ", $zero, 0"<<endl << " j " << finalLabel <<endl;
        ss<< label <<":"<<endl<< "addi " << temp << ", $zero, 1"<<endl<<finalLabel<<":"<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss << leftSideCode.code << endl
        << rightSideCode.code <<endl;
    }
    code.code = ss.str();
}

void GteExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    stringstream ss;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        ss<< "sge "<< temp<< ", "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< "c.ge.s "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
    }
    code.code = ss.str();
}

void LteExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    stringstream ss;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        ss<< "sle "<< temp<< ", "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< "c.le.s "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
    }
    code.code = ss.str();
}

void GtExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    stringstream ss;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        ss<< "sgt "<< temp<< ", "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< "c.gt.s "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
    }
    code.code = ss.str();
}

void LtExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    stringstream ss;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    if (leftSideCode.type == INT && rightSideCode.type == INT)
    {
        code.type = INT;
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        string temp = getIntTemp();
        ss<< "slt "<< temp<< ", "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
        code.place = temp;
    }else{
        code.type = FLOAT;
        toFloat(leftSideCode);
        toFloat(rightSideCode);
        ss << leftSideCode.code <<endl<< rightSideCode.code<<endl;
        releaseRegister(leftSideCode.place);
        releaseRegister(rightSideCode.place);
        ss<< "c.lt.s "<< leftSideCode.place<< ", "<< rightSideCode.place<<endl;
    }
    code.code = ss.str();
}

void LogicalAndExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    stringstream ss;
    ss<< leftSideCode.code<<endl << rightSideCode.code <<endl;
    releaseRegister(leftSideCode.place);
    releaseRegister(rightSideCode.place);
    string temp = getIntTemp();
    string label = getNewLabel("label");
    string finalLabel = getNewLabel("finalLabel");
    ss<< "addi "<<temp<< ", $zero, 0"<<endl;
    ss<< "beq "<< leftSideCode.place<< ", "<<temp<<", "<< finalLabel<<endl;
    ss<< "beq "<< rightSideCode.place<< ", "<<temp<<", "<< finalLabel<<endl;
    ss<< label<< ":"<<endl<< "addi "<< temp<< ", $zero, 1"<<endl<<finalLabel<<":"<<endl;
    code.place = temp;
    code.code = ss.str();
}

void LogicalOrExpr::genCode(Code &code){
    Code leftSideCode; 
    Code rightSideCode;
    this->expr1->genCode(leftSideCode);
    this->expr2->genCode(rightSideCode);
    stringstream ss;
    ss<< leftSideCode.code<<endl<< rightSideCode.code<<endl;
    releaseRegister(leftSideCode.place);
    releaseRegister(rightSideCode.place);
    string temp = getIntTemp();
    string label = getNewLabel("label");
    string finalLabel = getNewLabel("finalLabel");
    ss<< "addi "<<temp<< ", $zero, 1"<<endl;
    ss<< "beq "<< leftSideCode.place<< ", "<<temp <<", " << finalLabel <<endl;
    ss<< "beq "<<rightSideCode.place<< ", "<<temp <<", " << finalLabel <<endl;
    ss<< label <<":"<<endl<< "addi "<< temp<< ", $zero, 0"<<finalLabel<<":";
    code.place = temp;
    code.code = ss.str();
}

void AssignExpr::genCode(Code &code){
    Code rightSideCode;
    stringstream ss;
    this->expr2->genCode(rightSideCode);
    ss<< rightSideCode.code <<endl;
    string name = ((IdExpr *)this->expr1)->id;
    if(codeGenerationVars.find(name) == codeGenerationVars.end()){
        if(rightSideCode.type == INT)
            ss << "sw "<<rightSideCode.place << ", "<<name <<endl;
        else if(rightSideCode.type == FLOAT)
            ss << "s.s "<<rightSideCode.place << ", "<<name <<endl;
    }else{
        if(rightSideCode.type == INT)
            ss<< "sw "<< rightSideCode.place <<", "<<codeGenerationVars[name]->offset<<"($sp)"<<endl;
        else if(rightSideCode.type == FLOAT)
            ss<< "s.s "<< rightSideCode.place <<", "<<codeGenerationVars[name]->offset<<"($sp)"<<endl;
    }
    releaseRegister(rightSideCode.place);
    code.code = ss.str();
}

// void PlusAssignExpr::genCode(Code &code){
    
// }

// void MinusAssignExpr::genCode(Code &code){
    
// }

IMPLEMENT_BINARY_GET_TYPE(Add);
IMPLEMENT_BINARY_GET_TYPE(Sub);
IMPLEMENT_BINARY_GET_TYPE(Mul);
IMPLEMENT_BINARY_GET_TYPE(Div);
IMPLEMENT_BINARY_GET_TYPE(Mod);
// IMPLEMENT_BINARY_GET_TYPE(Assign);
// IMPLEMENT_BINARY_GET_TYPE(PlusAssign);
// IMPLEMENT_BINARY_GET_TYPE(MinusAssign);

IMPLEMENT_BINARY_ARIT_GEN_CODE(Add, '+');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Sub, '-');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Mul, '*');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Div, '/');
IMPLEMENT_BINARY_ARIT_GEN_CODE(Mod, '%');

IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Eq);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Neq);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Gte);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Lte);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Gt);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(Lt);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(LogicalAnd);
IMPLEMENT_BINARY_BOOLEAN_GET_TYPE(LogicalOr);