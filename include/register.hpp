#pragma once

#include "definitions.hpp"

class Register{
	byte value;
public:
	Register(byte b) : value(b) {}
	byte read();
	void write(byte b);
	void clear();
};