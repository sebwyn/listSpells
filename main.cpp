//defines most of the language including all the types and the base function 
//the function class in langInternals acts as the main execution system
//may move the old "interpreter file" to be a function method
//this would mean that code cannot be run outside of a function
#include "langInternals.h"

#include "lexer.h"
#include "parser.h"
#include "builtin.h"

#include "loadFile.h"

#include <vector>
#include <string>
#include <iostream>

using namespace mylang;
int main(int args, char** argv){
	if(args > 1){
		std::string codeString = loadFile(argv[1]);
		std::vector<token> tokens = lex(codeString);
		std::cout << "Found " << tokens.size() 
		<< " tokens" << std::endl;
		cell* firstExpr;
		
		if(parse(tokens, &firstExpr)){
			std::cout << "Parsed Successfully" << std::endl;
			std::string input;
			cell* currNode = firstExpr;
			//make a list of command line args 
			//that we can pass to the code
			cell* inArgs = NULL;
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
			//create the input to the main function	
			inArgs = new cell();
			inArgs->car = value(LIST, new cell());
			std::string quoteString = std::string("quote");
			value quoteArgValue = value(SYM, (void*)"quote");	
			((cell*)inArgs->car.v)->car=quoteArgValue;
			((cell*)inArgs->car.v)->cdr = terArgs;
			//create the builtin env
			env defEnv = mylang::env(); builtin::makeBuiltinEnv(&defEnv);

			//generate params for main func			
			value fArgs = value(LIST, new cell());
			((cell*)fArgs.v)->car=value(SYM,(void*)"args");
			func f = func(&defEnv, fArgs, firstExpr);
			
			value out; f.call(inArgs, &defEnv, &out);
			//destroy the code
			f.destroy();
			//destroying the main function will delete all the code making other other sub functions go out of scope	
			std::cout 
			<< "Program finished with return value:" << std::endl << out.repr() << std::endl;
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

