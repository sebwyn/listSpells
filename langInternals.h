#pragma once

#include "hashtable.h"

namespace mylang {

typedef enum type {
	INT,
	NIL,

	SYM,
	FUNC,
		
	LIST			
} type;

typedef struct cell {
	type t;
	void* car;
	cell* cdr;
	
	cell();
} cell;

typedef struct env {
	hashtable e;
	env* parent;

	env();
	env(env* parent);	
} env;

typedef struct func {
	cell* args;
	cell* code;
	env* parent;

	func();
	func(cell* args, cell* code, env* parent);		
} func;

}
