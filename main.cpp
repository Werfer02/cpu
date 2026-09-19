#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <cctype>

#include "coututils.hpp"

typedef uint8_t byte;
typedef uint8_t address;
const int byte_max = std::numeric_limits<byte>().max();

enum class operation{
	PROG_END,
	TEST,
	NOOP,
	ECHO,
	JUMP,
	WRITEHEAP,
	READHEAP,
	JUMPZERO,
	WRITEREG,
	ADD,
	SUB,
	WRITESCREEN,
	JUMPREG,
	JUMPZEROREG,
	LABEL,
	ROM,
	GET
};

std::unordered_map<byte, operation>  byteToOperation{
	{0b00000001,   operation::PROG_END},
	{0b00000010,       operation::TEST},
	{0b00000000,       operation::NOOP},
	{0b00000011,       operation::ECHO},
	{0b00000100,       operation::JUMP},
	{0b00000101,  operation::WRITEHEAP},
	{0b00000110,   operation::READHEAP},
	{0b00000111,   operation::JUMPZERO},
	{0b00001000,   operation::WRITEREG},
	{0b00001001,        operation::ADD},
	{0b00001010,        operation::SUB},
	{0b00001011,operation::WRITESCREEN},
	{0b00001100,    operation::JUMPREG},
	{0b00001101,operation::JUMPZEROREG},
	{0b00001110,      operation::LABEL},
	{0b00001111,        operation::ROM},
	{0b00010000,        operation::GET},
};

std::unordered_map<std::string, operation>  stringToOperation{
	{"PROGEND",       operation::PROG_END},
	{"TEST",              operation::TEST},
	{"NOOP",              operation::NOOP},
	{"ECHO",              operation::ECHO},
	{"JUMP",              operation::JUMP},
	{"WRITEHEAP",    operation::WRITEHEAP},
	{"READHEAP",      operation::READHEAP},
	{"JUMPZERO",      operation::JUMPZERO},
	{"WRITEREG",      operation::WRITEREG},
	{"ADD",                operation::ADD},
	{"SUB",                operation::SUB},
	{"WRITESCREEN",operation::WRITESCREEN},
	{"JUMPREG",        operation::JUMPREG},
	{"JUMPZEROREG",operation::JUMPZEROREG},
	{"LABEL",            operation::LABEL},
	{"ROM",                operation::ROM},
	{"GET",                operation::GET},
};

enum class operandType {
	BYTE,
	LABEL
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
    {operation::PROG_END,   {0b00000001, {}}},
    {operation::TEST,       {0b00000010, {}}},
    {operation::NOOP,       {0b00000000, {}}},
    {operation::ECHO,       {0b00000011, {operandType::BYTE, operandType::BYTE}}},  // lengthreg, stringadressreg
    {operation::JUMP,       {0b00000100, {operandType::LABEL}}},					// jumplabel
    {operation::WRITEHEAP,  {0b00000101, {operandType::BYTE, operandType::BYTE}}},  // addressreg, reg 
    {operation::READHEAP,   {0b00000110, {operandType::BYTE, operandType::BYTE}}},  // reg, addressreg 
    {operation::JUMPZERO,   {0b00000111, {operandType::BYTE, operandType::LABEL}}}, // zeroreg, jumplabel
    {operation::WRITEREG,   {0b00001000, {operandType::BYTE, operandType::BYTE}}},  // reg, value      <-- ideally this should be the only instruction to take literals
    {operation::ADD,        {0b00001001, {operandType::BYTE, operandType::BYTE}}},  // reg, reg
    {operation::SUB,        {0b00001010, {operandType::BYTE, operandType::BYTE}}},  // reg, reg
    {operation::WRITESCREEN,{0b00001011, {operandType::BYTE, operandType::BYTE}}},  // yreg, xreg (to write to screen, write x,y to these registers, write to regPixelR,G,B then call writescreen)
	{operation::JUMPREG,    {0b00001100, {operandType::BYTE}}},						// addressreg
	{operation::JUMPZEROREG,{0b00001101, {operandType::BYTE, operandType::BYTE}}},  // zeroreg, addressreg
	{operation::LABEL,      {0b00001110, {operandType::LABEL}}},					// label
	{operation::ROM,        {0b00001111, {operandType::BYTE}}},						// literal rom length
	{operation::GET,        {0b00010000, {operandType::BYTE, operandType::BYTE}}},	// reg, infobytereg (hardcoded info bytes for different possible info to get)
};

void printError(const std::vector<std::string>& args){
	std::cout << ansi::bright_red;
	std::cout << "ERROR: ";
	for(const std::string& arg : args){
		std::cout << arg;
	}
	std::cout << ansi::reset;
}

void printInfo(const std::vector<std::string>& args){
	std::cout << ansi::yellow;
	std::cout << "INFO: ";
	for(const std::string& arg : args){
		std::cout << arg;
	}
	std::cout << ansi::reset;
}

class Sim{
	static constexpr int HEAP_SIZE = 1*256;

	static constexpr int SCREEN_W = 32;
	static constexpr int SCREEN_H = 32;
	
	byte reg0 = 0;
	byte reg1 = 0;
	byte reg2 = 0;
	byte reg3 = 0;

	byte regPixelR = 0;
	byte regPixelG = 0;
	byte regPixelB = 0;
	std::array<std::string, (SCREEN_W * SCREEN_H)> regScreen;
	bool screenReady = false;

	byte heap[HEAP_SIZE] = {0};
	
	address progStart = 0;
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
		
		while(i < HEAP_SIZE){
			if(heap[i] == 0){ // this must mean free, size of a block is never zero, always jumping to size field
				if(i + bytes < HEAP_SIZE){
					heap[i] = bytes;
					heap[i+1] = true;
					return i+2;
				}
				i++;
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
		printError({"couldnt allocate\n"});
		return 0;
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
		progStart = a;
	}

	void initScreen(){
		regScreen.fill(" ");
		screenReady = true;
	}

	void run(){
		std::cout << "< program started >\n";
		address progCounter = progStart;
		while(progCounter < HEAP_SIZE){
			byte b = heap[progCounter];
			if (b == operationInfoMap[operation::PROG_END].binary) break;

			if(byteToOperation.find(b) == byteToOperation.end()){
				printError({"no operation for byte: '", std::to_string(b), "'\n"});
				return;
			}
			operation op = byteToOperation[b];
			switch(op){
				case operation::TEST : {
					std::cout << "test\n";
					break;
				}
				case operation::ECHO : {
					std::string echostr;
					progCounter++;
					int echolen = *getReg(heap[progCounter]);
					progCounter++;
					address echostart = *getReg(heap[progCounter]);
					address echoCounter = echostart;
					while(echoCounter < echostart + echolen) {
						echostr.push_back(heap[echoCounter]);
						echoCounter++;
					}
					std::cout << "echo: " << echostr << "\n";
					break;
				}
				case operation::JUMPREG : {
					progCounter++;
					//std::cout << "jump to: " << (int)heap[progCounter] << "\n";
					progCounter = *getReg(heap[progCounter]) + progStart;
					progCounter--; // go back one because loop goes forward one
					break;
				}
				case operation::WRITEHEAP : {
					progCounter++;
					address writeTo = *getReg(heap[progCounter]);
					progCounter++;
					heap[writeTo] = *getReg(heap[progCounter]);
					break;
				}
				case operation::READHEAP : {
					progCounter++;
					byte* reg = getReg(heap[progCounter]);
					progCounter++;
					*reg = heap[*getReg(heap[progCounter])];;
					break;
				}
				case operation::JUMPZEROREG : {
					progCounter++;
					byte* reg = getReg(heap[progCounter]);
					progCounter++;
					address jumpTo = *getReg(heap[progCounter]);
					if(*reg == 0){
						//std::cout << "jump to (zero): " << (int)jumpTo << "\n";
						progCounter = progStart + jumpTo;
						progCounter--; // go back one because loop goes forward one
					}
					break;
				}
				case operation::WRITEREG : {
					progCounter++;
					//std::cout << "writereg: " << (int)heap[progCounter] << "\n";
					byte* reg = getReg(heap[progCounter]);
					progCounter++;
					*reg = heap[progCounter];
					break;
				}
				case operation::ADD : {
					progCounter++;
					byte* firstReg  = getReg(heap[progCounter]);
					progCounter++;
					byte* secondReg = getReg(heap[progCounter]);
					*firstReg += *secondReg;
					break;
				}
				case operation::SUB : {
					progCounter++;
					byte* firstReg  = getReg(heap[progCounter]);
					progCounter++;
					byte* secondReg = getReg(heap[progCounter]);
					*firstReg -= *secondReg;
					break;
				}
				case operation::WRITESCREEN : {
					if(!screenReady) initScreen();
					progCounter++;
					int x = *getReg(heap[progCounter]);
					progCounter++;
					int y = *getReg(heap[progCounter]);
					regScreen[(y * SCREEN_W) + x] = ansi::rgb(regPixelR, regPixelG, regPixelB) + "█" + ansi::reset;
					break;
				}
				case operation::JUMP : {
					progCounter++;
					progCounter = progStart + heap[progCounter];
					progCounter--; // go back one because loop goes forward one
					break;
				}
				case operation::JUMPZERO : {
					progCounter++;
					byte* reg = getReg(heap[progCounter]);
					progCounter++;
					address jumpTo = heap[progCounter];
					if(*reg == 0){
						//std::cout << "jump to (zero): " << (int)jumpTo << "\n";
						progCounter = progStart + jumpTo;
						progCounter--; // go back one because loop goes forward one
					}
					break;
				}
				case operation::LABEL : {
					break;
				}
				case operation::ROM : { // at start of rom
					//std::cout << "at rom: " << (int)progCounter << "\n";
					progCounter++;
					progCounter += heap[progCounter] + 1; // skip data and this byte
					//std::cout << "now at byte: " << (int)progCounter << "\n";
					progCounter--; // go back one because loop goes forward one
					break;
				}
				case operation::GET : {
					progCounter++;
					byte* toReg = getReg(heap[progCounter]);
					progCounter++;
					byte infoByte = *getReg(heap[progCounter]);
					*toReg = getInfo(infoByte);
					break;
				}
			}
			progCounter++;
		}
		std::cout << "< program finished >\n";

	}

	byte getInfo(byte infoByte){
		switch(infoByte){
			case 'S' : {
				return progStart;
			}
			default : {
				printError({"no info for byte: '", std::to_string((char)infoByte), "'\n"});
				return 0;
			}
		}
	}

	byte* getReg(byte index){
		switch(index) {
			case 0   : return &reg0;
			case 1   : return &reg1;
			case 2   : return &reg2;
			case 3   : return &reg3;
			case 'R' : return &regPixelR;
			case 'G' : return &regPixelG;
			case 'B' : return &regPixelB;
			default : {
				printError({"no register: '", std::to_string((char)index), "'\n"});
				return nullptr;
			}
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

	void printRegisters(){
		std::cout << "reg0: " << (int)reg0 << "\n";
		std::cout << "reg1: " << (int)reg1 << "\n";
		std::cout << "reg2: " << (int)reg2 << "\n";
		std::cout << "reg3: " << (int)reg3 << "\n";
		std::cout << "regPixelR: " << (int)regPixelR << "\n";
		std::cout << "regPixelG: " << (int)regPixelG << "\n";
		std::cout << "regPixelB: " << (int)regPixelB << "\n";
	}

	void printScreen(){
		if(!screenReady){
			printInfo({"screen has not been written to, not printing\n"});
			return;
		}
		for(int i = 0; i < SCREEN_H; i++){
			for(int j = 0; j < SCREEN_W; j++){
				std::cout << regScreen[(i*SCREEN_W) + j];
			}
			std::cout << "\n";
		}
	}
};

std::vector<byte> readBinaryProgramFromFile(const std::string& path){
	std::ifstream file(path, std::ios::binary);
	std::vector<byte> prog;
	
	int length = 0;
	for(char c; c = file.get();){
		if(length >= byte_max) {
			printError({"program too long, addressable limit: ", std::to_string(byte_max), "\n"});
			return {0};
		}
		else prog.push_back((byte)c);
		length++;
	}
	file.close();
	return prog;
}

void writeBinaryProgramToFile(const std::string& path, const std::vector<byte>& prog){
	std::ofstream file(path, std::ios::binary);
	
	file.write(reinterpret_cast<const char*>(prog.data()), prog.size());
	file.close();
}

struct instruction {
	operation op;
	std::vector<operand> operands;
};

std::vector<instruction> readInstructionsFromFile(const std::string& path){
	std::ifstream file(path);
	std::vector<instruction> instructions;
	std::stringstream linestream;
	bool hasROM = false;

	int ROMcounter = 0;

	for(std::string line; std::getline(file, line, ';');){
		linestream.clear();
		linestream.str(line);
		std::string word;

		if(ROMcounter > 0){ // if reading rom
			line = line.substr(1); // skip first char (newline from formatting)
			if(line.length() == 1 && !isdigit(line[0])){ // if single non digit char
				instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, (byte)line[0]});
				ROMcounter--;
			} else try { 								 // try convert whole line to number
				size_t numendpos = 0;
				int num = (byte)std::stoi(line, &numendpos);
				if(numendpos == line.length()){			 // if whole line consumed as number
					instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, (byte)num});
					ROMcounter--;
				} else throw std::invalid_argument("stoi");
			}
			catch (std::invalid_argument){				 // write line byte by byte
				for(byte c : line){
					instructions[instructions.size() - 1].operands.push_back({operandType::BYTE, c});
					ROMcounter--;
				}
			}
		} else if(linestream >> word){ // first word
			if(stringToOperation.find(word) != stringToOperation.end()){
				instruction i;
				i.op = stringToOperation[word];
				if(i.op == operation::ROM){
					if(hasROM){
						printError({"more than one ROM instruction found\n"});
						return {};
					}
					linestream >> word;

					int a;
					if(word.length() == 1 && !isdigit(word[0])){
						a = word[0];
					} else {
						a = std::stoi(word);
					}
					if(a > byte_max){
						printError({"byte operand too big: ", std::to_string(a), "\n"});
						return {};
					} else i.operands.push_back({operandType::BYTE, (byte)a});

					hasROM = true;
					ROMcounter = a;
				}
				else while(linestream >> word && i.operands.size() < operationInfoMap[i.op].operands.size()){ // rest of words up to max operands
					switch(operationInfoMap[i.op].operands[i.operands.size()]){ // no -1 because looking for next operand
						case operandType::BYTE : {
							int a; 
							if(word.length() == 1 && !isdigit(word[0])){
								a = word[0];
							} else {
								a = std::stoi(word);
							}
							if(a > byte_max){
								printError({"byte operand too big: '", std::to_string(a), "'\n"});
								return {};
							} else i.operands.push_back({operandType::BYTE, (byte)a});
							break;
						}
						case operandType::LABEL : {
							i.operands.push_back({operandType::LABEL, 0, word});
							break;
						}
					}
				}
				instructions.push_back(i);
			}
			else {
				printError({"unknown operation: \"", word, "\"\n"});
			}
		}
	}
	file.close();
	return instructions;
}

std::vector<byte> instructionsToBinary(const std::vector<instruction>& instructions){
	std::vector<byte> prog;
	
	int position = 0;
	int labelcounter = 0;
	std::unordered_map<std::string, address> labelsdefined;
	std::unordered_map<int, address> labelsused;
	std::unordered_map<int, std::string> indextolabel;
	for(auto i : instructions){
		if(position >= byte_max) {
			printError({"program too long, addressable limit: '", std::to_string(byte_max), "'\n"});
			return {0};
		}
		if(i.op == operation::LABEL){
			if(i.operands.empty()){
				printError({"empty label\n"});
				return{0};
			}
			labelsdefined[i.operands[0].stringvalue] = position;
			continue;
		}
		else if(i.op == operation::ROM){
			prog.push_back(operationInfoMap[i.op].binary);
			prog.push_back(i.operands[0].bytevalue);
			for(int b = 1; b <= i.operands[0].bytevalue; b++){ // skip first operand, use it as length, need the <= !!!
				prog.push_back(i.operands[b].bytevalue);
			}
			position += 2 + i.operands[0].bytevalue; // operation + length byte + length of rom
			continue;
		}
		prog.push_back(operationInfoMap[i.op].binary);
		position++;
		for(auto op : i.operands) {
			switch(op.type){
				case operandType::BYTE : {
					prog.push_back(op.bytevalue);
					position++;
					break;
				}
				case operandType::LABEL : {
					labelsused.insert({labelcounter, position});
					indextolabel.insert({labelcounter, op.stringvalue});
					prog.push_back(0);
					labelcounter++;
					position++;
					break;
				}
			}
		}
	}
	for(auto l : labelsused){
		if(labelsdefined.find(indextolabel[l.first]) == labelsdefined.end()){
			printError({"undefined label used: ", std::to_string(l.first), "\n"});
		} else {
			prog[l.second] = labelsdefined[indextolabel[l.first]];
		}
	}
	return prog;
}

int main(int argc, char** argv){
	std::string file = "tests/romtest.inst";
	if(argc >= 2){
		file = argv[1];
	}
	Sim s;
	std::vector<instruction> instructions = readInstructionsFromFile(file);
	std::vector<byte> program = instructionsToBinary(instructions);

	address a = s.allocate(program.size()*sizeof(byte));
	if(a){
		s.write(program.data(), a, program.size() * sizeof(byte));
		s.setProgStart(a);
		s.printHeap();
		s.run();
		s.printScreen();
		std::cout << "registers:\n";
		s.printRegisters();
	} else{
		printError({"error allocating, got address 0\n"});
	}
}
