#pragma once

#include "langInternals.h"

namespace builtin {
//definitions of function subclasses
//this may also include some builtin macros 

class quoteFunc : mylang::func {
public:
	bool bindArgs(mylang::cell* args, mylang::env* targetEnv, mylang::env* callingEnv);
	void myCall(mylang::env* fEnv, mylang::value* out);

	quoteFunc(mylang::env* parent);
};

class anyArgsFunc : public mylang::func {
protected:
	bool anyBindArgs(mylang::cell* args, mylang::env* targetEnv, mylang::env* callingEnv);
public:
	//use this to wrap base constructor with params
	anyArgsFunc(); //required for children constructs
	anyArgsFunc(mylang::env* parent,mylang::value params,mylang::cell* code);
	bool bindArgs(mylang::cell* args, mylang::env* targetEnv, mylang::env* callingEnv);	
};

class lambdaFunc : public anyArgsFunc {
public:
	void myCall(mylang::env* fEnv, mylang::value* out);
	bool bindArgs(mylang::cell* args, mylang::env* targetEnv, mylang::env* callingEnv);
	lambdaFunc(mylang::env* parent);//lambda will only have one instance
};

//evaluate the body with env of calling func
class evalFunc : public anyArgsFunc {
public:
	void myCall(mylang::env* fEnv, mylang::value* out);
	bool bindArgs(mylang::cell* args, mylang::env* targetEnv, mylang::env* callingEnv);
	evalFunc(mylang::env* parent);

};

class listFunc : public anyArgsFunc {
public:
	void myCall(mylang::env* fEnv, mylang::value* out);
	listFunc(mylang::env* parent);
};

class globalFunc : public mylang::func {
public:
	void myCall(mylang::env* fEnv, mylang::value* out);
	globalFunc(mylang::env* parent);	
};

void makeBuiltinEnv(mylang::env* out);
}
