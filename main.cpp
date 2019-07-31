#include "langInternals.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

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
					std::cout << "type: " << currNode->t 
					<< " ";
					switch(currNode->t){
						case(INT):
							std::cout << "INT: " << 
							(*(int*)currNode->car);
							break;
						case(SYM):
							std::cout 
							<< currNode->car 
							<< " ";
							std::cout << "SYM: " 
							<< std::string(
							(char*)currNode->car);	
							break;
						case(LIST):
							std::cout << "SEXPR: " 
							<< (long)currNode->car;
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
			terArgs->t = LIST;
			terArgs->car = new cell();	
			cell* currArg = (cell*)terArgs->car;
			if(args > 2){
				for(int i = 2; i < args; i++){
					std::string stringArg(argv[i]);
					currArg->t = INT;
					currArg->car = malloc(sizeof(int));
					(*(int*)currArg->car) = std::stoi(stringArg);
					if(i != args - 1){
						currArg->cdr = new cell();
						currArg = currArg->cdr;	
					}	
				}
			}
			//add built-in functions to the environment
			std::string aString = std::string("a");
			std::string quoteString = std::string(std::string("quote").c_str());
			std::string argsString = std::string("args");
			env builtin = env();
			cell* quoteArgs = new cell();
			quoteArgs->t = SYM;
			quoteArgs->car = (void*)aString.c_str();
			func* quote = new func(quoteArgs, (cell*)0x1, &builtin);
			cell* quoteWrapper = new cell();
			quoteWrapper->t = FUNC;
			quoteWrapper->car = quote;
			builtin.e.add(quoteString, quoteWrapper);
	
			//make a function and call it with the args
			cell* inArgs = new cell();
			inArgs->t = LIST;
			inArgs->car = (void*)new cell();
			((cell*)inArgs->car)->t = SYM;
			((cell*)inArgs->car)->car = (void*)quoteString.c_str();
			((cell*)inArgs->car)->cdr = terArgs;
			cell* fArgs = new cell();
			fArgs->t = SYM;
			fArgs->car = (void*)argsString.c_str();
			func f = func( fArgs, firstExpr, &builtin);
			cell out = call(f, inArgs);
			std::cout 
			<< "Program finished with return type: " << out.t << " "
			<<  "and a value ptr of: " << out.car
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

