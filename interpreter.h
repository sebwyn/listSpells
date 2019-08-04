#pragma once

#include "langInternals.h"

mylang::value* allocVal(mylang::value literal);
mylang::value evalExpr(mylang::cell e, mylang::env environ);
