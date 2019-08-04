#include "langInternals.h"

#include <iostream>

#include "interpreter.h" //for evalExpr and allocVal
//these two funcs should be moved later

using namespace mylang;

value::value(){
	t = NIL;
	v = NULL;
}
void value::destroy(){
	switch(t){
		case(SYM):
			delete (char*)v;
			break;
		case(INT):
			delete (int*)v;
			break;
		case(FUNC):
			delete (func*)v;
			break;
		case(LIST):
			((cell*)v)->destroy();
			break;
	}
}

value::value(type t, void* v){
	this->t = t;
	this->v = v;
}

cell::cell(){
	car = value();
	cdr = NULL;
}

void cell::destroy(){
	//value should automatically be freed(its destructor should be called)
	//cdr needs to be freed though(if its not null)
	car.destroy();
	if(cdr != NULL){
		cdr->destroy();
	}	
}

env::env() {
	e = hashtable();
	parent = NULL;
}

env::env(env* parent){
	e = hashtable();
	this->parent = parent;
}

func::func(){
	params = value();
	code = NULL;
	parent = NULL;	
}

func::func(env* parent,value params,cell* code){
	if(params.t != LIST){
		std::cout << "improper function def" << std::endl;
		throw 1;
	} else {
		cell* currParam = (cell*)params.v;
		while(currParam){
			if(currParam->car.t != SYM){
				throw 1;
			} else {
				std::string key=std::string((char*)currParam->car.v);
				value temp;
				if(parent->e.get(key, &temp)){
					temp.destroy();
					std::cout
					<< "param name hides a parent name"
					<< std::endl;
					throw 1;
				}
				temp.destroy();	
			}
			currParam = currParam->cdr;
		}
	}
	this->params = params;
	this->parent = parent;
	this->code = code;
}

bool func::bindArgs(cell* args, env* fEnv){
	//ensure params is a list
	if(params.t != LIST){
		std::cout << "Invalid function" << std::endl;
		return false;
	}	
	cell* currParam = (cell*)params.v;
	cell* currArg = args; //expects a linked non-list of expressions
	while(currArg){
		if(currParam){
			if(currParam->car.t == SYM){
				std::string key=std::string((char*)currParam->car.v);
				value* val=allocVal(evalExpr((*currArg),*fEnv));
				if(!fEnv->e.add(key, val)){
					std::cout << "Strange Error" 
					<< std::endl;
					return false;
				}
				currParam = currParam->cdr;
				currArg = currArg->cdr;
			} else {
				std::cout << "Function has improper def" 
				<< std::endl; 
				return false;
			}
		} else {
			std::cout << "Not enough Args" << std::endl;
			return false;
		}
	}
	if(currParam != NULL){
		std::cout << "Expected More Args" << std::endl;
		return false;
	}
	return true;

}

void func::myCall(env* fEnv, value* out){
	value outVal = value();
	cell* currInstr = code;
	while(currInstr){
		outVal = evalExpr(*currInstr, fEnv);//def in interpreter
		currInstr = currInstr->cdr;
	}
	//allocate a new type to preserve code data
	(*out) = *(allocVal(outVal));
}

void func::call(cell* args, value* out){
	//allocate a new environment 
	//this will allow for closures
	env* fEnv = new env(parent);
	bindArgs(args, fEnv);
	//pass fEnv as a pointer so we can implement closures
	//check if the return type is a function and therefore we should save
	//the environment
	myCall(fEnv, out);
	if(out->t != FUNC){
		delete fEnv;
	}
	//NEED TO FIND A WAY TO FREE FLOATING ENVIRON
}

