#pragma once

#include <vector>

#include "langconfig.hpp"

struct instruction {
	operation op;
	std::vector<operand> operands;
};

class Assembler{
	LangConfig langConfig;

public:
	Assembler(const LangConfig& cfg);

	std::vector<instruction> readInstructionsFromFile(const std::string& path);

	std::vector<byte> instructionsToBinary(const std::vector<instruction>& instructions);
};