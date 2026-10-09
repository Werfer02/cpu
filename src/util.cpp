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
    printError(std::vector<std::string>{arg});
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
    printInfo(std::vector<std::string>{arg});
}

std::vector<byte> readBinaryFromFile(const std::string& path){
    std::ifstream file(path, std::ios::binary);
    return std::vector<byte>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

void writeBinaryToFile(const std::string& path, const std::vector<byte>& prog){
    std::ofstream file(path, std::ios::binary);
    for(byte b : prog){
        file << b;
    }
}