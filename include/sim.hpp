#pragma once

#include <array>

#include "langconfig.hpp"

struct SimConfig{
	const static int HEAP_SIZE = 1*256;
	const static int SCREEN_W = 32;
	const static int SCREEN_H = 32;

	LangConfig langConfig;
};

class Sim{

	SimConfig config;

	byte reg0 = 0;
	byte reg1 = 0;
	byte reg2 = 0;
	byte reg3 = 0;

	byte regPixelR = 0;
	byte regPixelG = 0;
	byte regPixelB = 0;
	std::array<std::string, SimConfig::SCREEN_W * SimConfig::SCREEN_H> regScreen;
	bool screenReady = false;
	bool screenWritten = false;

	byte heap[SimConfig::HEAP_SIZE] = {0};
	
	address progStart = 0;
	address progCounter = 0;
	// [size][occupied][data]
	
	address useBlock(address blockAddress, int bytes);

	void initScreen();

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

	byte* getReg(byte index);
	
	void printHeap();

	void printRegisters();

	void printScreen();
};