#include "interpreter.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace mylang;

cell* allocateType(cell literal){
	cell* out = new cell();
	switch(literal.t){
		case(INT):
			out->t = INT;
			out->car = malloc(sizeof(int));
			memcpy(out->car, literal.car, sizeof(int));
			break;
		case(NIL):
			break; //by default cell() creates a nil cell
		case(FUNC):
			out->t = FUNC;
			out->car = new func();
			memcpy(out->car, literal.car, sizeof(func));
			break;
		case(LIST): { //may cause a problem if the list is NULL 
			//this chunk of code will be gross
			//every time we allocate a list
			//we must copy a linked list
			//TO DO: Find a better way
			out->t = LIST;
			out->car = (void*)(new cell());
			cell* lastCell = NULL;
			cell* currDCell = (cell*)out->car;
			cell* currSCell = (cell*)literal.car;
			while(currSCell != NULL){
				memcpy(currDCell, currSCell, sizeof(cell));
				currDCell->cdr = (cell*)malloc(sizeof(cell));
				lastCell = currDCell;
				currDCell = currDCell->cdr;
				currSCell = currSCell->cdr;	
			}
			free(currDCell);
			lastCell->cdr = NULL;	
			break;
		}
		case(SYM):
			out->t = SYM;
			out->car = (void*)malloc(strlen(((char*)literal.car)) + 1);  
			strcpy(((char*)out->car), ((char*)(literal.car)));
			break;
	}
	return out;
}

cell evalExpr(cell e, env* environ){		
	cell out = cell();
	switch(e.t){
		case(LIST): { //evaluate the first element of the list
			cell op = evalExpr(*((cell*)e.car), environ);
			if(op.t == FUNC){
				cell* a = ((cell*)e.car)->cdr;
				out = call((*(func*)op.car), a);
			} else {
				std::cout << 
				"Expected function as first list element" 
				<< std::endl;
				throw 20;
			}	
			break;
		}
		case(SYM):{
			bool found = false;
			env* currEnv = environ;
			std::string key = std::string((char*)(e.car));
			while(currEnv != NULL){
				cell* c = (cell*)currEnv->e.get(key);
				if(c != NULL){
					out = (*c);
					found = true;
					break;
				}
				currEnv = currEnv->parent;	
			}
			if(!found){
				std::cout << "Symbol '" << key << 
				"' is not defined" 
				<< std::endl;
				throw 4;
			}
			break;
		}	
		default: //the type is some atomic constant
			out = e;
			out.cdr = NULL;
			break;	
	}
	return out;	
}

cell call(func f, cell* args){
	//make a new environment that points to the functions parent environ
	env fEnv = env(f.parent);

	//check to see if the function is quote
	//therefore we should not evaluate args (just return now)
	if(f.code == (void*)0x1){ //code is quote
		return (*args);
	}
	//variables for iterating through args evaluating them and binding 
	//them to parameter names
	cell* currArg = args;
	cell* currParam = f.args;
	//while there are still arguments provided 
	//bind them to function arg names
	
	//bind argument implementation
	while(currArg != NULL){
		if(currParam != NULL) {
			std::string varName=std::string((char*)currParam->car);	
			void* varVal = allocateType(evalExpr((*currArg), 
									&fEnv));
			if(!fEnv.e.add(varName, varVal)){
				std::cout 
				<< "A Strange internal error has occured: " 
				<< "An arg name could not be bound"
				<< std::endl;
				throw 3;	
			}
			currArg = currArg->cdr;
			currParam = currParam->cdr;
		} else {
			std::cout << 
			"Too many args supplied to function" 
			<< std::endl;
			throw 1;	 
		}	
	}
	//std::cout << "ehllo" << std::endl;
	if(currParam != NULL){
		std::cout << 
		"To few args supplied to function" 
		<< std::endl;
		throw 2;
	}

	//to do implement system functions with c++ bindings
	//lambda, list, quote
	cell out;
	switch((long)f.code){
		//built-in-funcs
		case((long)0x2): //lambda
			break;
		default:{

			//implement user-defined functions 
			//where expressions are successively
			//evaluated and the value of the 
			//last expression is returned
			cell* curr = f.code;
			while(curr != NULL){
				out = evalExpr((*curr), &fEnv);
				curr = curr->cdr;
			}
			break;
		}
	}
	return out;
}
