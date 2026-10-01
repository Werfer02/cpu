#pragma once

#include <array>

#include "simconfig.hpp"

class Screen{
	std::array<std::string, SimConfig::SCREEN_W * SimConfig::SCREEN_H> data;
	flag screenReady = false;
public:
	void init();
	void write(int x, int y, int r, int g, int b);
	void print();
};
