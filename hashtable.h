#pragma once

#include <string>
#include <vector>

#include "langInternals.h"

#define HASH_SIZE 50

typedef struct hashNode {
	std::string key;
	mylang::value* val;
	hashNode* next;

	hashNode(std::string key, mylang::value* val);
	~hashNode();
} hashNode;

class hashtable {
private:
	std::vector<hashNode*> table;
		
	unsigned int hash(std::string key);
public:
	
	hashtable();
	~hashtable();	
	
	bool add(std::string key, mylang::value* val);		
	bool set(std::string key, mylang::value* val);
	bool get(std::string key, mylang::value* out);
};
