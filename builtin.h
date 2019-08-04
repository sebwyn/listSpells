#pragma once

#include "langInternals.h"

namespace builtin {
//definitions of function subclasses
//this may also include some builtin macros 

class quoteFunc : mylang::func {
public:
	bool bindArgs(mylang::cell* args, mylang::env* fEnv);
	void myCall(mylang::env* fEnv, mylang::value* out);
};

class anyArgsFunc : public mylang::func {
public:
	//use this to wrap base constructor with params
	anyArgsFunc(); //required for children constructs
	anyArgsFunc(mylang::env* parent,mylang::value params,mylang::cell* code);
	bool bindArgs(mylang::cell* args, mylang::env* fEnv);	
};

class lambdaFunc : public anyArgsFunc {
public:
	void myCall(mylang::env* fEnv, mylang::value* out);
	lambdaFunc();//lambda will only have one instance
};

mylang::env makeBuiltinEnv();
}
