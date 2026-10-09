#pragma once

#include "langconfig.hpp"
#include "register.hpp"
#include "alu.hpp"
#include "screen.hpp"
#include "simconfig.hpp"

class Sim{

	SimConfig config;

	std::vector<Register> registers;

	ALU alu;

	Register regPixelR = 0;
	Register regPixelG = 0;
	Register regPixelB = 0;
	Screen screen;
	flag screenWritten = false;

	flag registersWritten = false;

	std::vector<byte> heap;

	flag memRead = false;
	flag memWrite = false;
	byte memAddr = 0;
	byte memValue = 0;
	
	address progStart = 0;
	address progCounter = 0;
	// [size][occupied][data]
	
	address useBlock(address blockAddress, int bytes);

public:

	Sim(SimConfig c = {});

	LangConfig getLangConfig();

	address allocate(byte bytes);
	
	void free(address tofree);
	void write(const void* memory, address location, int size);

	void setProgStart(address a);

	void run(address a);

	void run();

	byte getInfo(byte infoByte);

	Register& getReg(byte index);
	
	void printHeap();

	void printRegisters();

	void printScreen();
};