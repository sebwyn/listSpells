#include "langInternals.h"

#include <iostream>

#include <sstream>
#include <string>

#include "interpreter.h" //for evalExpr and allocVal
//these two funcs should be moved later

using namespace mylang;

value::value(){
	t = LIST;
	v = NULL;
}

value::value(type t, void* v){
	this->t = t;
	this->v = v;
}

std::string value::repr(){
	std::stringstream out;
	switch(t){
		case(INT):
			out << "INT " << (*(int*)v);
			break;
		case(SYM):
			out << "SYM " << ((char*)v);
			break;
		case(LIST):{
			cell* currCell = (cell*)v;
			if(currCell){
				//non-nil
				out << "LIST" << "\n";
				int count = 0;
				while(currCell){
					std::istringstream currCellStream; currCellStream.str(currCell->car.repr());
					std::string currLine; getline(currCellStream, currLine);
					out << '\t' <<  "CELL " << count++ << ": " << currLine << '\n';
					for(std::string cl; getline(currCellStream, cl);){
						out << "\t" << cl << "\n";
					}
					currCell = currCell->cdr;
				}
			} else {
				//NIL list
				out << "LIST NIL";
			}
			break;
		}
		case(FUNC):
			out << "FUNC at " << (long)v;
			break;	   
		
	}
	return out.str();
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
			if(v){
				((cell*)v)->destroy();
			}
			break;
	}
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
	params = value(); //inits params to nil (empty list)
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

//TO DO: iterate the other way through params
bool func::bindArgs(cell* args, env* targetEnv, env* callingEnv){
	//ensure params is a list
	cell* currParam = (cell*)params.v;
	cell* currArg = args; //expects a linked non-list of expressions
	while(currArg){
		if(currParam){
			if(currParam->car.t == SYM){
				std::string key=std::string((char*)currParam->car.v);
				value* val=allocVal(evalExpr((*currArg), callingEnv));
				if(!targetEnv->e.add(key, val)){
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

void func::call(cell* args, env* callingEnv, value* out){
	env* fEnv = new env(parent);
	if(bindArgs(args, fEnv, callingEnv)){
		myCall(fEnv, out);
		//temporarily allow for closures
		//we will really need to keep an env ref count
		if(out->t != FUNC){
			delete fEnv;	
		}
	} else {
		throw 1;
	}
}

void func::destroy(){
	code->destroy();
}
