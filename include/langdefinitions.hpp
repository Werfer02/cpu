#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "definitions.hpp"

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
	GET,
	COPY
};

extern std::unordered_map<std::string, operation>  stringToOperation;

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