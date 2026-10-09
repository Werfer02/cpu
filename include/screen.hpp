#pragma once

#include <array>

#include "simconfig.hpp"

class Screen{
	std::vector<std::string> data;
	flag screenReady = false;
	int width = 1, height = 1;
public:
	Screen();
	Screen(int w, int h);
	void write(int x, int y, int r, int g, int b);
	void print();
};
