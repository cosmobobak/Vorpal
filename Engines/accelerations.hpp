#pragma once

#include <bitset>

inline auto popcount(const int_fast16_t bb) -> int_fast8_t {
    return __builtin_popcount(bb);
}

template <class T>
auto string(std::vector<T> v) -> std::string {
    std::string builder;
    builder.append("{ ");
    for (auto &&i : v) {
        builder.append(std::to_string(i));
        builder.append(", ");
    }
    builder.append("}");
    return builder;
}

auto lsb(int_fast16_t bitboard) -> int_fast8_t {
    return __builtin_ffs(bitboard) - 1;
}