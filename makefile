all:
	g++ -std=c++11 main.cpp lexer.cpp parser.cpp hashtable.cpp interpreter.cpp langInternals.cpp builtin.cpp -o mylang

debug:
	g++ -std=c++11 -g main.cpp lexer.cpp parser.cpp hashtable.cpp interpreter.cpp langInternals.cpp builtin.cpp -o mylang
