#pragma once

#include "langconfig.hpp"

struct SimConfig{
	int heapSize = 1*256;
	int screenW = 32;
	int screenH = 32;
	int numRegisters = 16;

	LangConfig langConfig;
};