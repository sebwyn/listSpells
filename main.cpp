//defines most of the language including all the types and the base function 
//the function class in langInternals acts as the main execution system
//may move the old "interpreter file" to be a function method
//this would mean that code cannot be run outside of a function
#include "langInternals.h"

#include "lexer.h"
#include "parser.h"

#include "builtin.h"

#include <vector>
#include <string>
#include <iostream>

using namespace mylang;
int main(int args, char** argv){
	if(args > 1){
		std::vector<token> tokens = lex(std::string(argv[1]));
		std::cout << "Found " << tokens.size() 
		<< " tokens" << std::endl;
		cell* firstExpr;
		
		if(parse(tokens, &firstExpr)){
			std::cout << "Parsed Successfully" << std::endl;
			std::string input;
			cell* currNode = firstExpr;
			while(input != "q"){
				if(input == "current"){
					std::cout << "type: " << currNode->car.t 					<< " ";
					switch(currNode->car.t){
						case(INT):
							std::cout << "INT: " << 
							*(int*)currNode->car.v;
							break;
						case(SYM):
							std::cout 
							<< currNode->car.v 
							<< " ";
							std::cout << "SYM: " 
							<< std::string(
							(char*)currNode->car.v);
							break;
						case(LIST):
							std::cout << "SEXPR: " 
							<<(long)currNode->car.v;
							break;
					} 
					std::cout << std::endl;
				} else if(input == "next"){
					if(currNode->cdr != NULL){
						currNode = currNode->cdr;
					}
				} else if(input == "start"){
					currNode = firstExpr;
				}
				std::cin >> input;
			}
			//make a list of command line args 
			//that we can pass to the code
			cell* terArgs = new cell();
			terArgs->car = value(LIST, new cell());	
			cell* currArg = (cell*)(terArgs->car.v);
			if(args > 2){
				for(int i = 2; i < args; i++){
					std::string strArg=std::string(argv[i]);
					currArg->car = value(INT, new int);
					int currInt = std::stoi(strArg);
					*((int*)(currArg->car.v)) = currInt;
					if(i != args - 1){
						currArg->cdr = new cell();
						currArg = currArg->cdr;	
					}	
				}
			}
			//add built-in functions to the environment
			env defEnv = builtin::makeBuiltinEnv();
			
			//quote terArgs so they can be passed to the main func
			cell* inArgs = new cell();
			inArgs->car = value(LIST, new cell());
			std::string quoteString = std::string("quote");
			value quoteArgValue = value(SYM, (void*)"quote");	
			((cell*)inArgs->car.v)->car=quoteArgValue;
			((cell*)inArgs->car.v)->cdr = terArgs;
			
			//generate params for main func			
			value fArgs = value(LIST, new cell());
			((cell*)fArgs.v)->car=value(SYM,(void*)"args");
			func f = func(&defEnv, fArgs, firstExpr);
			
			value out; f.call(inArgs, &out);
			std::cout 
			<< "Program finished with return type: " << out.t << " "
			<<  "and a value ptr of: " << out.v
			<< std::endl;
		} else {
			std::cout << "Failed to parse" << std::endl;
			return 1;
		}
	} else {
		std::cout << "Expected an argument" << std::endl;
		return 1;
	}
	return 0;
}

