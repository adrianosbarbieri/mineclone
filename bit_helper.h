#pragma once
#include <cstdint>

void set(int8_t& cell, int8_t flag);

int8_t has(int8_t cell, int8_t flag);

void toggle(int8_t& cell, int8_t flag);

void clear(int8_t& cell, int8_t flag);