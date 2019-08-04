#include "builtin.h"
#include "interpreter.h"

#include <vector>

using namespace builtin;
using namespace mylang;

bool generateParams(std::vector<std::string> strings, value* out){
	out->t = LIST;
	out->v = new cell();
	cell* currParam = (cell*)out->v;
	for(int i = 0; i < strings.size(); i++){
		currParam->car = value(SYM, (void*)strings[i].c_str());
		if(i != strings.size() - 1){
			currParam->cdr = new cell();
			currParam = currParam->cdr; 
		} 
	}		
}

bool quoteFunc::bindArgs(cell* args, env* fEnv){
	if(args){
		if(!args->cdr){
			std::string key=std::string("a");
			value* allocatedRawArg = 
				allocVal(value(args->car.t, args->car.v)); 
			fEnv->e.add(key, allocatedRawArg);
			return true;		
		}
	}	
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
	//ensure there is at least one param
	if(params.v == NULL){
		throw 1;
	}
}

bool anyArgsFunc::bindArgs(cell* args, env* fEnv){
	cell* currArg = args;
	cell* currParam = (cell*)params.v;
	
	value* rest = new value(LIST, (void*)(new cell()));
	cell* currRest = (cell*)rest->v;
	while(currArg){
		std::string key = std::string((char*)currParam->car.v);
		if(currParam->cdr){
			value* val = allocVal(evalExpr((*currArg), *fEnv));
			fEnv->e.add(key, val);		
			currParam = currParam->cdr;
		} else {
			value* currRestVal=allocVal(evalExpr((*currArg),*fEnv));
			currRest->car=*currRestVal;
			if(currArg->cdr != NULL){
				//there is another node after this
				currRest->cdr = new cell();
				currRest = currRest->cdr;	
			} else {
				//there is not another node we should end
				fEnv->e.add(key, rest);
				currParam = currParam->cdr;
				break;
			}	
		}	
		currArg = currArg->cdr;
	}
	if(currParam != NULL){
		return false;		
	}
	return true;
} 

lambdaFunc::lambdaFunc(){
	//args should be a list
	//body is just made from the statements following the lambda
	//however these statements will be evaluated so they must be quoted
	std::vector<std::string> p = {"params", "body"};
	generateParams(p, &params);
	//code will be inited to NULL
	//parent will init to NULL
}

void lambdaFunc::myCall(env* fEnv, value* out){
	//will return a function
	//instantiate a new function
	//WARNING: we are banking on the parent env surviving
	//when we keep a reference to body
	//this may lead to problems
	value p; fEnv->e.get(std::string("params"), &p);
	value c; fEnv->e.get(std::string("body"), &c);	
	func* userFunc = new func(fEnv, p, (cell*)c.v);
	(*out)=value(FUNC, userFunc); 
}

env builtin::makeBuiltinEnv(){
	env out;
	//values need to be allocated so that ptrs wont be freed when 
	//the xxxxFuncVals leave scope
	std::string quoteString = std::string("quote");
	value* quoteFuncVal = new value(FUNC, (void*)new quoteFunc());
	out.e.add(quoteString, quoteFuncVal);
	std::string lambdaString = std::string("lambda");
	value* lambdaFuncVal = new value(FUNC, (void*)new lambdaFunc());
	out.e.add(lambdaString, lambdaFuncVal);
	return out;		
}
