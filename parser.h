#pragma once

#include "langInternals.h"
#include "lexer.h" //include to get access to tokens and tokentypes

#include <string>
#include <vector>

mylang::cell decodeToken(token t);
bool parse(std::vector<token> tokens, mylang::cell** out);

