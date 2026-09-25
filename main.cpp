#include <iostream>
#include <vector>

#include "coututils.hpp"

#include "sim.hpp"
#include "assembler.hpp"
#include "util.hpp"

int main(int argc, char** argv){
	std::string file = "tests/echoromtest.inst";
	if(argc >= 2){
		file = argv[1];
	}

	Sim s;
	Assembler as(s.getLangConfig());

	std::vector<byte> program = as.instructionsToBinary(as.readInstructionsFromFile(file));

	address a = s.allocate(program.size()*sizeof(byte));

	if(a){
		s.write(program.data(), a, program.size() * sizeof(byte));
		//s.printHeap();
		s.run(a);
		s.printScreen();
		s.printRegisters();
	} else{
		printError("error allocating, got address 0\n");
	}
}
