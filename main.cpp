#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <algorithm>

#include "coututils.hpp"

typedef uint8_t byte;
typedef uint16_t address;
const int byte_max = std::numeric_limits<byte>().max();

enum class operation{
	PROG_END,
	TEST,
	NOOP,
	ECHO,
	JUMP,
	WRITE
};

std::unordered_map<byte, operation>  byteToOperation{
	{0b00000001, operation::PROG_END},
	{0b00000010,     operation::TEST},
	{0b00000000,     operation::NOOP},
	{0b00000011,     operation::ECHO},
	{0b00000100,     operation::JUMP},
	{0b00000101,    operation::WRITE},
};

std::unordered_map<std::string, operation>  stringToOperation{
	{"PROGEND",  operation::PROG_END},
	{"TEST",         operation::TEST},
	{"NOOP",         operation::NOOP},
	{"ECHO",         operation::ECHO},
	{"JUMP",        operation::WRITE},
};

enum class operandType {
	BYTE,
	STRING
};

struct operand {
	operandType type;
	byte bytevalue;
	std::string stringvalue;
};

struct operationInfo {
	byte binary;
	std::vector<operandType> operands;
};

std::unordered_map<operation, operationInfo> operationInfoMap{
    {operation::PROG_END, {0b00000001, {}}},
    {operation::TEST,     {0b00000010, {}}},
    {operation::NOOP,     {0b00000000, {}}},
    {operation::ECHO,     {0b00000011, {operandType::BYTE, operandType::STRING}}},
    {operation::JUMP,     {0b00000100, {operandType::BYTE}}},
    {operation::WRITE,    {0b00000101, {operandType::BYTE, operandType::BYTE}}},
};

class Sim{
	const static int HEAP_SIZE = 1*1028;
	
	byte reg1 = 0;
	byte reg2 = 0;
	byte reg3[4] = {0};
	byte reg4[8] = {0};
	byte regScreen[32 * 32 * 3] = {0};
	byte heap[HEAP_SIZE] = {0};
	
	address progCounter = 0;
	// [size][occupied][data]
	
	address useBlock(address blockAddress, int bytes){
		int blockSize = heap[blockAddress];
		
		if(blockSize >= bytes + 3){ // bytes + metadata + at least 1 usable byte ? then split into 2 blocks
			heap[blockAddress+2+bytes] = blockSize - (bytes + 2); // second block with leftover length
			heap[blockAddress+2+bytes+1] = false; // mark as free
			
			heap[blockAddress] = bytes;  
			heap[blockAddress+1] = true; // first block
			return blockAddress+2;	
		}
		else { // if not enough space to make another usable block, just take up this whole block
			heap[blockAddress+1] = true;
			return blockAddress+2;
		}	
	}

public:

	address allocate(byte bytes){
		int i = 0;
		int mergeCandidate = -1;
		while(i < HEAP_SIZE && i < byte_max){
			if(heap[i] == 0){ // this must mean free, size of a block is never zero, always jumping to size field
				heap[i] = bytes;
				heap[i+1] = true;
				return i+2;
			}
			else if(heap[i] >= bytes){ // always jumping to size field
				if(heap[i+1] == false){ // this must mean its free
					return useBlock(i, bytes);
				} else{ // this must mean its occupied
					i += heap[i] + 2; // jump to next size field or free
				}
			}
			else if(heap[i] < bytes){ // always jumping to size field
				if(heap[i+1] == false) { // this block is too small but free so look to merge it
					if(mergeCandidate > -1){ // if previous block is merge candidate, merge
						heap[mergeCandidate] = heap[mergeCandidate] + heap[i] + 2;
						if(heap[mergeCandidate] >= bytes){ // if enough space stop looking
							return useBlock(mergeCandidate, bytes);
						}
					}
					else { // otherwise make this the merge candidate
						mergeCandidate = i; 
					}
				} else{
					mergeCandidate = -1;
				}
				i += heap[i] + 2; // jump to next size field or free
			}
		}
		std::cerr << "couldn't allocate\n";
		return -1;
	}
	
	void free(address tofree){
		heap[tofree-1] = false;
	}
	
	void write(const void* memory, address location, int size){
		const byte* src = (const byte*)memory;
		for(int i = 0; i < size; i++){
			heap[location + i] = src[i];
		}
	}

	void setProgStart(address a){
		progCounter = a;
	}

	void run(){
		byte b = heap[progCounter];
		address progStart = progCounter;
		while(progCounter < HEAP_SIZE && b != operationInfoMap[operation::PROG_END].binary){
			b = heap[progCounter];
			operation op = byteToOperation[b];
			switch(op){
				case operation::TEST : {
					std::cout << "test\n";
					break;
				}
				case operation::ECHO : {
					std::string echostr;
					progCounter++;
					int echolen = heap[progCounter];
					address echostart = progCounter;
					while(progCounter < echostart + echolen) {
						progCounter++;
						echostr.push_back(heap[progCounter]);
					}
					std::cout << "echo: " << echostr << "\n";
					break;
				}
				case operation::JUMP : {
					progCounter++;
					std::cout << "jump to: " << progStart + heap[progCounter] << "\n";
					progCounter = progStart + heap[progCounter];
					progCounter--; // go back one because loop goes forward one
				}
			}
			progCounter++;
		}
	}
	
	void printHeap(){
		int emptyCounter = 0;
		for(int i = 0; i < HEAP_SIZE; i++){
			char c = heap[i];
	
			if (c == '\0') {
				emptyCounter++;
				if(emptyCounter < 4){
					std::cout << i << ": " << "\\0\n";	
				}
				else if(emptyCounter == 4) {
					std::cout << ". . .\n";
					continue;
				}
			}
			else {
				emptyCounter = 0;
				
				std::cout << i << ": ";
	
				if (std::isprint(c)) std::cout << "'" << c << "'";
				else if (c == '\n') std::cout << "\\n";
				else std::cout << (int)c;
				
				std::cout << "\n";
			}
			
		}
	}
};

std::vector<byte> readBinaryProgramFromFile(const std::string& path){
	std::ifstream file(path, std::ios::binary);
	std::vector<byte> prog;
	
	int length = 0;
	for(char c; file >> c;){
		if(c == operationInfoMap[operation::PROG_END].binary) break;
		else if(length >= byte_max) {
			std::cerr << ansi::red << "program too long, addressable limit: " << byte_max << ansi::reset << "\n";
			return {0};
		}
		else prog.push_back((byte)c);
		length++;
	}
	file.close();
	return prog;
}

struct instruction {
	operation op;
	std::vector<operand> operands;
};

std::vector<instruction> readInstructionsFromFile(const std::string& path){
	std::ifstream file(path);
	std::vector<instruction> instructions;
	std::stringstream linestream;
	
	for(std::string line; std::getline(file, line, ';');){
		linestream.clear();
		linestream.str(line);
		std::string word;
		if(linestream >> word){ // first word
			if(stringToOperation.find(word) != stringToOperation.end()){
				instruction i;
				i.op = stringToOperation[word];
				while(linestream >> word && i.operands.size() < operationInfoMap[i.op].operands.size()){ // rest of words up to max operands
					switch(operationInfoMap[i.op].operands[i.operands.size()]){ // no -1 because looking for next operand
						case operandType::BYTE:{
							int a = std::stoi(word);
							if(a > byte_max){
								std::cerr << ansi::red << "byte operand too big: " << a << ansi::reset << "\n";
								return {};
							} else i.operands.push_back({operandType::BYTE, (byte)a});
							break;
						}
						case operandType::STRING:{
							std::string stringOperand = word;
							std::string rest;
							getline(linestream, rest); // get rest of line
							stringOperand.append(rest);
							i.operands.push_back({operandType::STRING, 0, stringOperand});
							std::cout << stringOperand << "\n";
							break;
						}
					}
				}
				instructions.push_back(i);
			}
			else {
				std::cerr << ansi::red << "unknown operation: \"" << word << "\"" << ansi::reset << "\n";
			}
		}
	}
	file.close();
	return instructions;
}

std::vector<byte> instructionsToBinary(const std::vector<instruction>& instructions){
	std::vector<byte> prog;
	
	for(auto i : instructions){
		prog.push_back(operationInfoMap[i.op].binary);
		for(auto op : i.operands) {
			switch(op.type){
				case operandType::BYTE:{
					prog.push_back(op.bytevalue);
				}
				case operandType::STRING:{
					for(auto c : op.stringvalue) prog.push_back((byte)c);
				}
			}
		}
	}
	return prog;
}

int main(){
	Sim s;
	std::vector<instruction> instructions = readInstructionsFromFile("instructions.inst");
	std::vector<byte> program = instructionsToBinary(instructions);

	address a = s.allocate(program.size()*sizeof(byte));
	s.write(program.data(), a, program.size() * sizeof(byte));
	s.setProgStart(a);
	s.printHeap();
	s.run();
	
}
