#pragma once

#include <bitset>

inline auto popcount(const short bb) -> short {
    return std::bitset<16>(bb).count();
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

auto lsb(short bitboard) -> int {
    return __builtin_ffs((int)bitboard) - 1;
}