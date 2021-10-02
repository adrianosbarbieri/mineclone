#pragma once
#include <cstdint>

inline void set(int8_t& cell, int8_t flag)
{
    cell |= flag;
}

inline int8_t has(int8_t cell, int8_t flag)
{
    return cell & flag;
}

inline void toggle(int8_t& cell, int8_t flag)
{
    cell ^= flag;
}

inline void clear(int8_t& cell, int8_t flag)
{
    cell &= ~(flag);
}