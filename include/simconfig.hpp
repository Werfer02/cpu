#pragma once

#include "langconfig.hpp"

struct SimConfig{
	const static int HEAP_SIZE = 1*256;
	const static int SCREEN_W = 32;
	const static int SCREEN_H = 32;

	LangConfig langConfig;
};