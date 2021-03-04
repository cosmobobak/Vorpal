#pragma once

#include "names.hpp"

// A bitscan forward is used to find the index of the least significant 1 bit (LS1B).
auto bitscan_forward(U64 bb) -> Square {
    return (Square)__builtin_ctzll(bb);
}

// A bitscan reverse is used to find the index of the most significant 1 bit (MS1B).
auto bitscan_reverse(U64 bb) -> Square {
    return (Square)__builtin_clzll(bb);
}

// note: ctz vs clz builtins in the above functions

// counts set bits
auto popcount(U64 bb) -> int {
    return __builtin_popcountll(bb);
}