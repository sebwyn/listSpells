#include "lexer.h"

token::token(tokenType type, std::string value){
	this->type = type;
	this->value = value;
}

std::vector<token> lex(std::string input){
		
	std::vector<token> out;

	std::string currName = std::string("");
	for(int i = 0; i < input.length(); i++){
		char c = input.at(i);
		std::string stringC = std::string(1, c);
		bool isSpecial = c == '(' || c == ')' || c == ' ';
		if(currName.length() > 0 && isSpecial){
			out.push_back(token(NAME, currName));
			currName = std::string("");
		}
		switch(c){
			case '(':
				out.push_back(token(OPEN_PAREN, stringC));
				break;
			case ')':
				out.push_back(token(CLOSE_PAREN, stringC));
				break;
			default:
				if(c != '\t' && c != '\n' && c != ' '){
					currName += std::string(1, input.at(i));
				}
				break;	
		}
	}
	if(currName.length() > 0){
		out.push_back(token(NAME, currName));
	}
	return out;
}
