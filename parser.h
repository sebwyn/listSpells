#pragma once

#include "langInternals.h"
#include "lexer.h" //include to get access to tokens and tokentypes

#include <vector>

bool parse(std::vector<token> tokens, mylang::cell** out);

