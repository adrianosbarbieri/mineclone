#include "bit_helper.h"

void set(int8_t& cell, int8_t flag)
{
    cell |= flag;
}

int8_t has(int8_t cell, int8_t flag)
{
    return cell & flag;
}

void toggle(int8_t& cell, int8_t flag)
{
    cell ^= flag;
}

void clear(int8_t& cell, int8_t flag)
{
    cell &= ~(flag);
}