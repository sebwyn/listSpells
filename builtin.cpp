#include "builtin.h"
#include "interpreter.h"

#include <iostream>
#include <vector>

using namespace builtin;
using namespace mylang;

//had to change to allocate new strings because orig strings will go out of reference
bool generateParams(std::vector<std::string> strings, value* out){
	out->t = LIST;
	out->v = new cell();
	cell* currParam = (cell*)out->v;
	for(int i = 0; i < strings.size(); i++){
		currParam->car = value(SYM, new char[strings[i].length()+1]);
		memcpy(currParam->car.v, strings[i].c_str(), strings[i].length()+1);
		if(i != strings.size() - 1){
			currParam->cdr = new cell();
			currParam = currParam->cdr; 
		} 
	}
	return true;	
}

quoteFunc::quoteFunc(env* parent) : func(parent, value(), NULL){
}

bool quoteFunc::bindArgs(cell* args, env* targetEnv, env* callingEnv){
	if(args){
		if(!args->cdr){
			std::string key=std::string("a");
			value* allocatedRawArg = allocVal(value(args->car.t, args->car.v)); 
			targetEnv->e.add(key, allocatedRawArg);
			return true;		
		}
	}
	std::cout << "Invalid Args supplied to quote" << std::endl;	
	return false;
}

void quoteFunc::myCall(env* fEnv, value* out){
	//allocate a new value because the value in fEnv will go out 
	//of scope
	//this may lead to problems or the inability to free the allocated 
	//value
	value gotValue; fEnv->e.get(std::string("a"), &gotValue);
	(*out) = *(allocVal(gotValue)); 
}

anyArgsFunc::anyArgsFunc(){
}

anyArgsFunc::anyArgsFunc(env* parent, value params, cell* code) : 
func(parent, params, code) {
}

bool anyArgsFunc::anyBindArgs(cell* args, env* targetEnv, env* callingEnv){
	if(params.t != LIST){
		return false;
	}
	cell* currArg = args;
	cell* currParam = (cell*)params.v;
	
	value* rest = new value(LIST, (void*)(new cell()));
	cell* currRest = (cell*)rest->v;
	while(currArg){
		std::string key = std::string((char*)currParam->car.v);
		if(currParam->cdr){
			value* val = allocVal(evalExpr((*currArg), callingEnv));
			targetEnv->e.add(key, val);		
			currParam = currParam->cdr;
		} else {
			value* currRestVal=allocVal(evalExpr((*currArg), callingEnv));
			currRest->car=*currRestVal;
			if(currArg->cdr != NULL){
				//there is another node after this
				currRest->cdr = new cell();
				currRest = currRest->cdr;	
			} else {
				//there is not another node we should end
				targetEnv->e.add(key, rest);
				currParam = currParam->cdr;
				break;
			}	
		}	
		currArg = currArg->cdr;
	}
	if(currParam != NULL){
		std::cout << "Not enough args supplied to anyArgsFunc" << std::endl;
		return false;		
	}
	return true;
}

bool anyArgsFunc::bindArgs(cell* args, env* targetEnv, env* callingEnv){
	return anyBindArgs(args, targetEnv, callingEnv);
}

lambdaFunc::lambdaFunc(env* parent) : anyArgsFunc(parent, value(), NULL){
	//args should be a list
	//body is just made from the statements following the lambda
	//however these statements will be evaluated so they must be quoted
	std::vector<std::string> p = {"params", "body"};
	//the calling function env will secretely be passed to lambda
	generateParams(p, &params);
	//code will be inited to NULL
	//parent will init to NULL
}

bool lambdaFunc::bindArgs(cell* args, env* targetEnv, env* callingEnv){
	if(anyBindArgs(args, targetEnv, callingEnv)){
		std::string secretEnvString = std::string("callingEnv");
		value* secretEnvVal = new value();
		secretEnvVal->v = callingEnv;
		targetEnv->e.add(secretEnvString, secretEnvVal);
		return true;
	} else {
		return false;
	}
}

void lambdaFunc::myCall(env* fEnv, value* out){
	//will return a function
	//instantiate a new function
	//WARNING: we are banking on the parent env surviving
	//when we keep a reference to body
	//this may lead to problems
	value eVal; fEnv->e.get(std::string("callingEnv"), &eVal);
	env* e = (env*)eVal.v;
	value p; fEnv->e.get(std::string("params"), &p);
	value c; fEnv->e.get(std::string("body"), &c);	
	func* userFunc = new func(e, p, (cell*)c.v);
	(*out)=value(FUNC, userFunc); 
}

evalFunc::evalFunc(env* parent) : anyArgsFunc(parent, value(), NULL) {	
	std::vector<std::string> p = {"body"};
	generateParams(p, &params);
}

bool evalFunc::bindArgs(cell* args, env* targetEnv, env* callingEnv){
	if(anyBindArgs(args, targetEnv, callingEnv)){
		std::string key = std::string("callingEnv");
		value* callingEnvVal = new value();
		callingEnvVal->v = callingEnv;
	       	targetEnv->e.add(key, callingEnvVal);
		return true;
	} else {
		return false;
	}
}

void evalFunc::myCall(env* fEnv, value* out){
	value callingEnvVal; fEnv->e.get("callingEnv", &callingEnvVal);
	env* callingEnv = (env*)(callingEnvVal.v);
	//body should be a list because of how anyArgsFunc works
	value body; fEnv->e.get("body", &body);
	
	cell* currInstr = ((cell*)body.v);
	while(currInstr){
		(*out) = evalExpr(*currInstr, callingEnv);
		currInstr = currInstr->cdr;
	}
			
}

listFunc::listFunc(env* parent) : anyArgsFunc(parent, value(), NULL) {
	std::vector<std::string> p = {"body"};
	generateParams(p, &params);
}

void listFunc::myCall(env* fEnv, value* out){
	//by default anyBindArgs should create desired output
	value l; fEnv->e.get("body", &l);
	(*out) = *(allocVal(l));
}

globalFunc::globalFunc(env* parent) : func(parent, value(), NULL) {
	std::vector<std::string> p = {"symbol", "value"};
	generateParams(p, &params);
}

void globalFunc::myCall(env* fEnv, mylang::value* out){
	//the parent is the builtin env
	value keyVal; fEnv->e.get("symbol", &keyVal);
	if(keyVal.t == SYM){
		std::string key = std::string((char*)keyVal.v);
		value val; fEnv->e.get("value", &val);
		value* allocatedVal = allocVal(val);
		parent->e.add(key, allocatedVal);

		(*out) = *(allocVal(keyVal));
} else {
		std::cout << "Global expects a symbol as its first arg" << std::endl;
		throw 1;
	}
}

addFunc::addFunc(env* parent) : anyArgsFunc(parent, value(), NULL){
	std::vector<std::string> p = {"x", "addends"};
	generateParams(p, &params);
}

void addFunc::myCall(env* fEnv, value* out){
	value x; fEnv->e.get("x", &x);
	value addends; fEnv->e.get("addends", &addends);
	if(x.t == INT){
		int sum = *((int*)x.v);
		cell* currAddend = (cell*)addends.v;
		while(currAddend){
			if(currAddend->car.t == INT){
				sum += *((int*)currAddend->car.v);	
			} else {
				std::cout << "Add expects addends to be integers" << std::endl;
				throw 1;
			}
			currAddend = currAddend->cdr;
		}

		(*out) = *(allocVal(value(INT, &sum)));
	} else {
		std::cout << "Add expects an integer" << std::endl;
		throw 1;
	}
}

void builtin::makeBuiltinEnv(env* out){
	//values need to be allocated so that ptrs wont be freed when 
	//the xxxxFuncVals leave scope
	std::string quoteString = std::string("quote");
	value* quoteFuncVal = new value(FUNC, (void*)new quoteFunc(out));
	out->e.add(quoteString, quoteFuncVal);
	std::string lambdaString = std::string("lambda");
	value* lambdaFuncVal = new value(FUNC, (void*)new lambdaFunc(out));
	out->e.add(lambdaString, lambdaFuncVal);
	std::string evalString = std::string("eval");
	value* evalFuncVal = new value(FUNC, (void*)new evalFunc(out));
	out->e.add(evalString, evalFuncVal);
	std::string globalString = std::string("global");
	value* globalFuncVal = new value(FUNC, (void*)new globalFunc(out));
	out->e.add(globalString, globalFuncVal);
	std::string listString = std::string("list");
	value* listFuncVal = new value(FUNC, (void*)new listFunc(out));
	out->e.add(listString, listFuncVal);
	std::string addString = std::string("add");
	value* addFuncVal = new value(FUNC, (void*)new addFunc(out));
	out->e.add(addString, addFuncVal);
}
