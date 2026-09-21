#include "util.hpp"

#include <iostream>
#include "coututils.hpp"

void printError(const std::vector<std::string>& args){
	std::cout << ansi::bright_red;
	std::cout << "ERROR: ";
	for(const std::string& arg : args){
		std::cout << arg;
	}
	std::cout << ansi::reset;
}
void printError(const std::string& arg){
    printError({arg});
}

void printInfo(const std::vector<std::string>& args){
	std::cout << ansi::yellow;
	std::cout << "INFO: ";
	for(const std::string& arg : args){
		std::cout << arg;
	}
	std::cout << ansi::reset;
}
void printInfo(const std::string& arg){
    printInfo({arg});
}