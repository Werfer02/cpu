#pragma once

#include "definitions.hpp"
#include "langdefinitions.hpp"

struct LangConfig{
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
		{operation::COPY,       {0b00010001, {operandType::BYTE, operandType::BYTE}}},	// toreg, fromreg
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
		{0b00010001,       operation::COPY},
	};
};