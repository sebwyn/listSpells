#include "langInternals.h"

using namespace mylang;

cell::cell(){
	t = NIL;
	car = NULL;
	cdr = NULL;
}

env::env() {
	e = hashtable();
	parent = NULL;
}

env::env(env* parent){
	e = hashtable();
	this->parent = parent;
}

func::func(){
	args = NULL;
	code = NULL;
	parent = NULL;	
}
func::func(cell* args, cell* code, env* parent){
	this->args = args;
	this->code = code;
	this->parent = parent;
}
