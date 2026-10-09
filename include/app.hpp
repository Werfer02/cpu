#pragma once

#include <string>

#include "sim.hpp"

class App{
	bool wantsToRunProgram 		= true;
	bool wantsToOutputAssembly 	= false;
	bool wantsToPrintHeapBefore = false;
	bool wantsToPrintHeapAfter  = false;
	bool wantsToPrintRegisters 	= false;

	bool binaryInput = false;

	std::string inputProgramPath = "";
	std::string binaryOutputPath = "";

	Sim s;

public:
	void configure(int argc, char** argv);
	void run();
};