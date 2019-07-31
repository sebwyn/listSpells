#include "parser.h"

#include <iostream>
#include <cstdint>
#include <cstring>
#include <sstream>

using namespace mylang;

cell decodeToken(token t){
	
	cell out = cell();	
	if(0x2F < t.value.at(0) && t.value.at(0) < 0x3A){
		std::stringstream s(t.value);
		int* value = new int;
		s >> (*value);
		out.t = INT;
		out.car = value; 		
	} else {
		char* value = new char[t.value.length()];
		std::memcpy(value, (void*)t.value.c_str(), t.value.length());
		out.t = SYM;
		out.car = value;	
	}
	return out;
}

bool parse(std::vector<token> tokens, cell** out){
	
	std::vector<cell*> firstExpressions;
	std::vector<cell*> lastExpressions;
	cell* lastExpr = NULL;
	cell* currExpr = new cell();
	firstExpressions.push_back(currExpr);
	for(int i = 0; i < tokens.size(); i++){
		if(tokens[i].type == OPEN_PAREN){
			//push the current cell so we can return to it when we 
			//find a closing parenthesis
			lastExpressions.push_back(currExpr);
			//push a new list and point to the first 
			//cell in it
			cell* e = new cell();
			firstExpressions.push_back(e);
			//reset the last expression to NULL
			//we dont have to store this for when we pop
			//because we already know the last expression has a cdr
			lastExpr = NULL; 
			currExpr = e;	
		} else if(tokens[i].type == CLOSE_PAREN){
			if(firstExpressions.size() > 1){	
				//make the previous node point to null, 
				//because there is no following list element
				if(lastExpr == NULL){
					//the list is empty, make a NULL type
					delete firstExpressions.back();
					firstExpressions.pop_back();
					currExpr = lastExpressions.back();
					lastExpressions.back();
					currExpr->cdr = new cell();
					lastExpr = currExpr;
					currExpr = currExpr->cdr;	
				} else {
					lastExpr->cdr = NULL;
					//store the root node of the current 
					//list and pop it
					cell* lastE = firstExpressions.back();
					firstExpressions.pop_back();
					//set the currentExpr to be the 
					//expression that we pushed 
					//before entering this list
					currExpr = lastExpressions.back();
					lastExpressions.pop_back();
		
					currExpr->t = LIST;
					currExpr->car = lastE;
				
					//make a new node and have 
					//currExpr point to it
					currExpr->cdr = new cell();
					lastExpr = currExpr;	
					currExpr = currExpr->cdr;
				}
			} else {
				std::cout << "Unexpected Paren" << std::endl;
				return false;
			}	
		} else {
			(*currExpr) = decodeToken(tokens[i]);	
			
			currExpr->cdr = new cell();
			lastExpr = currExpr;
			currExpr = currExpr->cdr; 
		}
	}
	if(firstExpressions.size() > 1){
		std::cout << "Expected paren" << std::endl;
		return false;
	}
	lastExpr->cdr = NULL;
	(*out) = firstExpressions[0];	
	return true;
}
