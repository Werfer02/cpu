#pragma once

#include <cstdint>
#include <limits>

using byte = uint8_t;
using address = uint8_t;
using flag = bool;
const int BYTE_MAX = std::numeric_limits<byte>().max();