#include "hashtable.h"

hashNode::hashNode(std::string key, mylang::value* val){
	this->key = key;
	this->val = val;
	next = NULL;
}

hashNode::~hashNode(){
	if(next){
		delete next;
	}
	val->destroy();
	delete val;
}

hashtable::hashtable(){
	table = std::vector<hashNode*>(HASH_SIZE, (hashNode*)NULL);
}

hashtable::~hashtable(){
	for(int i = 0; i < HASH_SIZE; i++){
		if(table[i]){
			delete table[i];
		}
	}
}

unsigned int hashtable::hash(std::string key){
	unsigned long hashValue = 5381;
	for(int i = 0; i < key.length(); i++){
		hashValue = (hashValue << 5) + hashValue + key.at(i);
	}
	return hashValue % HASH_SIZE;
}

bool hashtable::add(std::string key, mylang::value* val){
	unsigned int h = hash(key);
	hashNode* lastNode = NULL;
	hashNode* currNode = table[h];
	while(currNode){
		if(currNode->key == key){
			//the table already has a value at key
			return false;
		}
		lastNode = currNode;
		currNode = currNode->next;	
	}
	if(lastNode){
		lastNode->next = new hashNode(key, val);
	} else {
		table[h] = new hashNode(key, val);
	}
	return true;
}

bool hashtable::set(std::string key, mylang::value* val){
	bool found = false;
	
	unsigned int h = hash(key);
	hashNode* currNode = table[h];
	while(currNode){
		if(currNode->key == key){
			delete currNode->val;
			currNode->val = val;
			found = true;
			break;
		}
		currNode = currNode->next;
	}
	return found;
}

bool hashtable::get(std::string key, mylang::value* out){
	bool found = false;

	unsigned int h = hash(key);
	hashNode* currNode = table[h];
	while(currNode){
		if(currNode->key == key){
			(*out) = (*(currNode->val));	
			found = true;
			break;
		}
		currNode = currNode->next;
	}
	return found;
}
