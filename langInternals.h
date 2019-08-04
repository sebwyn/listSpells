#pragma once

namespace mylang {

typedef enum type {
	INT,
	NIL,

	SYM,	
	LIST,

	FUNC
} type;

class value {
public:
	type t;
	void* v;
	
	void destroy();
	
	value();
	value(type t, void* v);
};

class cell {
public:
	value car;
	cell* cdr;
	
	cell();
	void destroy();
};

}

#include "hashtable.h"
//dont try to include this in include from hashtable
#ifdef HASH_SIZE 

namespace mylang {

typedef struct env {
	hashtable e;
	env* parent;

	env();
	env(env* parent);	
} env;

class func {
protected:
	value params; //expects a list 
	env* parent; //the environment the function was declared in
	cell* code; //points to a linked node, "code"
public:
	//this will require some reference to evaluate (a non class member)
	//or we could have an interpreter be a class and a func be a child?
	
	//lets us change the way args are read
	virtual bool bindArgs(cell* args, env* fEnv);

	//this will determine how args are interpreted and how functions
	//are executed
	//in most situations this will be a function that just interprets "code"
	//which points to user defined code
	//but this may also be bound to external cpp functions that modify
	//behaviour
	virtual void myCall(env* fEnv, value* out);
	//myCall wrapper that is common to all funcs
	void call(cell* args, value* out);
	
	func(); //default
	//mylang function with bindings in mylang
	//built-in func with binding in c++
	func(env* parent,value params,cell* code);
};
}

#endif
