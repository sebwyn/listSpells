#pragma once

#include <string>
#include <vector>

#define HASH_SIZE 50

typedef struct hashNode {
	std::string key;
	void* value;

	hashNode(std::string key, void* value);
} hashNode;

//important this implementation of a hash table requires values be allocated
//before being added
//when the hash table is destroyed or leaves scope the hash table will
//automatically free those allocated variables
class hashtable {
private:
	std::vector< std::vector<hashNode> > table;
	int getIndex(std::string key, unsigned int h);	
		
	unsigned int hash(std::string key);
public:
	
	hashtable();
	~hashtable();
	
	bool add(std::string key, void* value);		
	bool set(std::string key, void* value);
	void* get(std::string key);
};
