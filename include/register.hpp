#pragma once

#include "definitions.hpp"

class Register{
	byte value = 0;
public:
	Register() {}
	Register(byte b) : value(b) {}
	byte read();
	void write(byte b);
	void clear();
};