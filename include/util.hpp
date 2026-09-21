#pragma once

#include <vector>
#include <string>

#include "definitions.hpp"

void printError(const std::vector<std::string>& args);
void printError(const std::string& arg);

void printInfo(const std::vector<std::string>& args);
void printInfo(const std::string& arg);

std::vector<byte> readBinaryProgramFromFile(const std::string& path);

void writeBinaryProgramToFile(const std::string& path, const std::vector<byte>& prog);