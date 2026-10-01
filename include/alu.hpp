#pragma once

#include "definitions.hpp"

class ALU{

	flag isZero = 0;
	flag isPos = 0;
	flag isNeg = 0;

	byte result = 0;
	void setFlags();
public:
	byte add(byte a, byte b);
	byte subtract(byte a, byte b);
	byte multiply(byte a, byte b);
	byte divide(byte a, byte b);

	byte getResult();

	flag getZeroFlag();
	flag getPosFlag();
	flag getNegFlag();
};