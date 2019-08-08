#include "loadFile.h"

std::string loadFile(std::string name){
	std::string out = std::string("");
	std::ifstream inFile; inFile.open(name);
	if(!inFile){
		throw 1;
	}
	std::string line;
	while(std::getline(inFile, line)){
		out += line + '\n';
	}
	inFile.close();
	return out;
}
