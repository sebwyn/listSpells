#include "hashtable.h"

hashNode::hashNode(std::string key, void* value){
	this->key = key;
	this->value = value;
}

hashtable::hashtable(){
	table = std::vector< std::vector<hashNode> >(HASH_SIZE);
}

hashtable::~hashtable(){
	for(int h = 0; h < HASH_SIZE; h++){
		for(int i = 0; i < table[h].size(); i++){
			free(table[h][i].value);
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

int hashtable::getIndex(std::string key, unsigned int h){
	for(int i = 0; i < table[h].size(); i++){
		if(table[h][i].key == key){
			return i;
		}
	}
	return -1;
}

bool hashtable::add(std::string key, void* value){
	unsigned int h = hash(key);
	if(getIndex(key, h) < 0){ //make sure the given key doesn't exist
		table[h].push_back(hashNode(key, value));
		return true;	
	} else {
		return false;
	}
}

bool hashtable::set(std::string key, void* value){
	unsigned int h = hash(key);
	int i = getIndex(key, h);
	if(i > -1){
		free(table[h][i].value);
		table[h][i].value = value;
		return true;
	} else {
		return false;
	}
}

void* hashtable::get(std::string key){
	void* value = NULL;
	unsigned int h = hash(key);
	int i = getIndex(key, h);
	if(i > -1){
		return table[h][i].value;
	} else {
		return NULL;
	}
}
