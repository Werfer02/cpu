#pragma once

#include <array>

#include "langconfig.hpp"
#include "register.hpp"
#include "alu.hpp"
#include "screen.hpp"
#include "simconfig.hpp"

class Sim{

	SimConfig config;

	std::array<Register, SimConfig::NUM_REGISTERS> registers;

	ALU alu;

	Register regPixelR = 0;
	Register regPixelG = 0;
	Register regPixelB = 0;
	Screen screen;
	flag screenWritten = false;

	flag registersWritten = false;

	byte heap[SimConfig::HEAP_SIZE] = {0};

	flag memRead = false;
	flag memWrite = false;
	byte memAddr = 0;
	byte memValue = 0;
	
	address progStart = 0;
	address progCounter = 0;
	// [size][occupied][data]
	
	address useBlock(address blockAddress, int bytes);

public:

	Sim();

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