#include "interpreter.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace mylang;

//creates a copy of a type 
value* allocVal(value literal){
	value* out = new value();
	switch(literal.t){
		case(INT):
			out->t = INT;
			out->v = (void*)(new int);
			memcpy(out->v, literal.v, sizeof(int));
			break;
		case(FUNC):
			out->t = FUNC;
			out->v = (void*)(new func());
			//hopefully take advantage of copy semantics
			*((func*)out->v) = *((func*)literal.v);
			break;
		case(LIST): {  
			//this chunk of code will be gross
			//every time we allocate a list
			//we must copy a linked list
			//TO DO: Find a better way
			if(literal.v){ //make sure the value is not NULL
				out->v = (void*)(new cell());
				cell* lastCell = NULL;
				cell* currDCell = (cell*)out->v;
				cell* currSCell = (cell*)literal.v;
				while(currSCell != NULL){
					currDCell->car = *(allocVal(currSCell->car));
					currDCell->cdr = new cell();
					lastCell = currDCell;
					currDCell = currDCell->cdr;
					currSCell = currSCell->cdr;	
				}
				delete currDCell;
				lastCell->cdr = NULL;
			}	
			break;
		}
		case(SYM):
			out->t = SYM;
			out->v=(void*)(new char[strlen(((char*)literal.v))+1]);
			strcpy(((char*)out->v), ((char*)(literal.v)));
			break;
	}
	return out;
}

//an expression will never modify the environment 
//may want to change this
value evalExpr(cell e, env* environ){
	value out = value();
	switch(e.car.t){
		case(LIST): { //evaluate the first element of the list
			if(e.car.v){ //make sure the list is not null
				value op = evalExpr(*((cell*)e.car.v), environ);
				if(op.t == FUNC){
					//replaces old func::call
					cell* a = ((cell*)e.car.v)->cdr;
					((func*)op.v)->call(a, environ, &out); //environ must be passed so args can be evaled with it
				} else {
					std::cout << 
					"Expected function as first list element" 
					<< std::endl;
					throw 20;
				}
			} //else dont change value which should default to an empty list	
			break;
			}
		case(SYM):{
			bool found = false;
			env* currEnv = environ;
			std::string key = std::string((char*)(e.car.v));
			while(currEnv != NULL){
				if(currEnv->e.get(key, &out)){
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
			out = e.car;
			break;	
	}
	return out;	
}
