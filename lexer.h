#pragma once

#include <vector>
#include <string>

#include <string>
#include <vector>


typedef enum tokenType {
	OPEN_PAREN,
	CLOSE_PAREN,
	SPACE,
	NAME
} tokenType;

typedef struct token {
	tokenType type;
	std::string value;

	token(tokenType type, std::string value); 
} token;

std::vector<token> lex(std::string input);
