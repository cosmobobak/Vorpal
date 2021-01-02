#pragma once

#include <bitset>

auto popcount(short bb) -> short
{
    return std::bitset<16>(bb).count();
}